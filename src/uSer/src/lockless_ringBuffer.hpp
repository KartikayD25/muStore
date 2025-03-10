#pragma once

#include <atomic>
#include <cstdint>
#include <memory>
#include <vector>

// A simple lockless ring buffer implementation
// Uses atomic indices to avoid locks
template <typename T> class LocklessRingBuffer {
private:
  std::vector<T> buffer;
  std::atomic<size_t> head; // Write index
  std::atomic<size_t> tail; // Read index
  const size_t capacity;

  // Convert actual index to buffer index (handles wrap-around)
  size_t maskIndex(size_t index) const { return index % capacity; }

public:
  explicit LocklessRingBuffer(size_t size)
      : buffer(size), head(0), tail(0), capacity(size) {}

  // Check if the buffer is empty
  bool isEmpty() const {
    return head.load(std::memory_order_acquire) ==
           tail.load(std::memory_order_acquire);
  }

  // Check if the buffer is full
  bool isFull() const {
    return (head.load(std::memory_order_acquire) + 1) % capacity ==
           tail.load(std::memory_order_acquire);
  }

  // Count of items in the buffer
  size_t count() const {
    size_t h = head.load(std::memory_order_acquire);
    size_t t = tail.load(std::memory_order_acquire);
    if (h >= t) {
      return h - t;
    } else {
      return capacity - (t - h);
    }
  }

  // Try to enqueue an item (non-blocking)
  // Returns true if successful, false if buffer is full
  bool tryEnqueue(const T &item) {
    size_t currentHead = head.load(std::memory_order_relaxed);
    size_t nextHead = (currentHead + 1) % capacity;

    // Check if buffer is full
    if (nextHead == tail.load(std::memory_order_acquire)) {
      return false;
    }

    // Store item at current head
    buffer[currentHead] = item;

    // Update head (release ensures the item is visible to consumers)
    head.store(nextHead, std::memory_order_release);
    return true;
  }

  // Try to dequeue an item (non-blocking)
  // Returns true if successful, false if buffer is empty
  bool tryDequeue(T &item) {
    // Check if buffer is empty
    if (isEmpty()) {
      return false;
    }

    size_t currentTail = tail.load(std::memory_order_relaxed);

    // Read item at current tail
    item = buffer[currentTail];

    // Update tail (release ensures proper synchronization with producers)
    tail.store((currentTail + 1) % capacity, std::memory_order_release);
    return true;
  }
};