#include <memory>
#include <vector> 
#include <cstring> 
#include <array>
#include <atomic>
#include <thread>

#define NUM_SIZE_CLASSES 7
#define MMAP_THRESHOLD 4096

class MemoryPool {
private:
    static constexpr std::array<size_t, 4> SIZE_CLASSES = {64, 256, 1024, 4096};
    static constexpr size_t NUM_SLABS_PER_CLASS = 32;
    static constexpr float DEFRAG_THRESHOLD = 0.3f;
    static constexpr size_t MAX_DEFRAG_SLABS = 4;

    // Platform-specific optimized bit scanning
    static inline int find_first_zero(uint64_t x) {
        #if defined(__GNUC__) || defined(__clang__)
            return __builtin_ffsll(~x) - 1;
        #elif defined(_MSC_VER)
            unsigned long index;
            if (_BitScanForward64(&index, ~x)) {
                return static_cast<int>(index);
            }
            return -1;
        #else
            for (int i = 0; i < 64; i++) {
                if (!(x & (1ULL << i))) {
                    return i;
                }
            }
            return -1;
        #endif
    }

    struct Slab {
        char* memory;
        size_t size;
        size_t block_size;
        size_t num_blocks;
        size_t bitmap_size;  // Number of uint64_t words needed
        std::atomic<uint64_t>* bitmap;
        std::atomic<size_t> used_blocks;
        std::atomic<uint32_t> state;

        static constexpr uint32_t STATE_NORMAL = 0;
        static constexpr uint32_t STATE_DEFRAGGING = 1;

        Slab(size_t slab_size, size_t block_sz) 
            : size(slab_size), block_size(block_sz), used_blocks(0), state(STATE_NORMAL) {
            memory = new char[size];
            num_blocks = size / block_size;
            bitmap_size = (num_blocks + 63) / 64;
            bitmap = new std::atomic<uint64_t>[bitmap_size]();
        }

        ~Slab() {
            delete[] memory;
            delete[] bitmap;
        }

        int find_free_block() {
            for (size_t i = 0; i < bitmap_size; i++) {
                uint64_t current = bitmap[i].load(std::memory_order_acquire);
                if (~current != 0) {  // Has free bits
                    int bit_pos = find_first_zero(current);
                    if (bit_pos != -1) {
                        uint64_t mask = 1ULL << bit_pos;
                        // Try to set the bit atomically
                        while (true) {
                            if (bitmap[i].compare_exchange_weak(
                                current,
                                current | mask,
                                std::memory_order_acq_rel
                            )) {
                                used_blocks.fetch_add(1, std::memory_order_relaxed);
                                return i * 64 + bit_pos;
                            }
                            // If bit is already set, find next free bit
                            if (current & mask) {
                                bit_pos = find_first_zero(current);
                                if (bit_pos == -1) break;
                                mask = 1ULL << bit_pos;
                            }
                        }
                    }
                }
            }
            return -1;
        }

        void free_block(size_t block_index) {
            size_t word_index = block_index / 64;
            size_t bit_index = block_index % 64;
            uint64_t mask = 1ULL << bit_index;

            uint64_t old_val = bitmap[word_index].fetch_and(~mask, std::memory_order_acq_rel);
            if (old_val & mask) {
                used_blocks.fetch_sub(1, std::memory_order_relaxed);
            }
        }

        float fragmentation() const {
            size_t gaps = 0;
            size_t used = used_blocks.load(std::memory_order_acquire);
            
            for (size_t i = 0; i < bitmap_size; i++) {
                uint64_t word = bitmap[i].load(std::memory_order_acquire);
                uint64_t prev_bit = 1;  // Assume virtual 1 before first bit
                
                while (word) {
                    uint64_t bit = word & 1;
                    if (prev_bit && !bit) gaps++;
                    prev_bit = bit;
                    word >>= 1;
                }
            }
            
            return used > 0 ? static_cast<float>(gaps) / used : 0.0f;
        }

        bool try_start_defrag() {
            uint32_t expected = STATE_NORMAL;
            return state.compare_exchange_strong(
                expected,
                STATE_DEFRAGGING,
                std::memory_order_acq_rel
            );
        }

        void finish_defrag() {
            state.store(STATE_NORMAL, std::memory_order_release);
        }

        bool is_defragging() const {
            return state.load(std::memory_order_acquire) == STATE_DEFRAGGING;
        }
    };

    struct ThreadBuffer {
        std::array<char*, SIZE_CLASSES.size()> buffers;
        std::array<size_t, SIZE_CLASSES.size()> remaining;

        ThreadBuffer() {
            for (size_t i = 0; i < SIZE_CLASSES.size(); i++) {
                buffers[i] = nullptr;
                remaining[i] = 0;
            }
        }

        ~ThreadBuffer() {
            for (auto ptr : buffers) delete[] ptr;
        }
    };

    static thread_local ThreadBuffer thread_buffer;
    std::vector<std::vector<Slab>> slabs;
    std::atomic<size_t> active_defrags{0};

    void defragment_slab(size_t class_index, size_t slab_index) {
        Slab& slab = slabs[class_index][slab_index];
        
        if (!slab.try_start_defrag()) return;
        
        active_defrags.fetch_add(1, std::memory_order_acq_rel);

        char* temp = new char[slab.size];
        size_t write_offset = 0;

        // Scan bitmap and compact allocated blocks
        for (size_t i = 0; i < slab.num_blocks; i++) {
            size_t word_index = i / 64;
            size_t bit_index = i % 64;
            uint64_t word = slab.bitmap[word_index].load(std::memory_order_acquire);
            
            if (word & (1ULL << bit_index)) {
                std::memcpy(
                    temp + write_offset,
                    slab.memory + (i * slab.block_size),
                    slab.block_size
                );
                write_offset += slab.block_size;
            }
        }

        // Copy back compacted memory
        std::memcpy(slab.memory, temp, write_offset);
        delete[] temp;

        active_defrags.fetch_sub(1, std::memory_order_acq_rel);
        slab.finish_defrag();
    }

public:
    MemoryPool() : slabs(SIZE_CLASSES.size()) {
        for (size_t i = 0; i < SIZE_CLASSES.size(); i++) {
            slabs[i].reserve(NUM_SLABS_PER_CLASS);
            for (size_t j = 0; j < NUM_SLABS_PER_CLASS; j++) {
                slabs[i].emplace_back(SIZE_CLASSES[i] * 64, SIZE_CLASSES[i]);
            }
        }
    }

    void* allocate(size_t size) {
        size_t class_index = 0;
        for (; class_index < SIZE_CLASSES.size(); class_index++) {
            if (SIZE_CLASSES[class_index] >= size) break;
        }
        
        if (class_index >= SIZE_CLASSES.size()) {
            return ::operator new(size);
        }

        // Check thread-local buffer
        if (thread_buffer.remaining[class_index] >= size) {
            void* ptr = thread_buffer.buffers[class_index] + 
                       (SIZE_CLASSES[class_index] - thread_buffer.remaining[class_index]);
            thread_buffer.remaining[class_index] -= size;
            return ptr;
        }

        // Try to allocate from slabs
        for (size_t i = 0; i < slabs[class_index].size(); i++) {
            Slab& slab = slabs[class_index][i];
            
            if (slab.is_defragging()) continue;

            int block_index = slab.find_free_block();
            if (block_index != -1) {
                // Check if defrag is needed
                if (slab.fragmentation() > DEFRAG_THRESHOLD &&
                    active_defrags.load(std::memory_order_acquire) < MAX_DEFRAG_SLABS) {
                    std::thread([this, class_index, i]() {
                        defragment_slab(class_index, i);
                    }).detach();
                }

                return slab.memory + (block_index * slab.block_size);
            }
        }

        return nullptr;
    }

    void deallocate(void* ptr, size_t size) {
        for (size_t i = 0; i < SIZE_CLASSES.size(); i++) {
            if (SIZE_CLASSES[i] >= size) {
                for (size_t j = 0; j < slabs[i].size(); j++) {
                    Slab& slab = slabs[i][j];
                    if (ptr >= slab.memory && ptr < slab.memory + slab.size) {
                        if (!slab.is_defragging()) {
                            size_t offset = static_cast<char*>(ptr) - slab.memory;
                            size_t block_index = offset / slab.block_size;
                            slab.free_block(block_index);

                            if (slab.fragmentation() > DEFRAG_THRESHOLD &&
                                active_defrags.load(std::memory_order_acquire) < MAX_DEFRAG_SLABS) {
                                std::thread([this, i, j]() {
                                    defragment_slab(i, j);
                                }).detach();
                            }
                        }
                        return;
                    }
                }
                break;
            }
        }
        
        ::operator delete(ptr);
    }
};

thread_local MemoryPool::ThreadBuffer MemoryPool::thread_buffer;