#ifndef COMMON_SEPARATED
#define COMMON_SEPARATED

#include <iostream>
#include <cstring>
#include <unordered_map>
#include <cstdlib>
#include <ctime>
#include <fstream> 
#include <vector> 
#include <papi.h>
#include <sys/mman.h>  // For mmap and related constants (MAP_SHARED, PROT_READ)
#include <sys/stat.h>  // For struct stat and fstat
#include <fcntl.h>     // For open and O_RDONLY
#include <string.h>    // For strerror
#include <errno.h>  
#include <unistd.h>
#define STRING_COUNT 1000

#define INT_COUNT 1000
#define SELECTED_INT_COUNT 100

#define DOUBLE_COUNT 1000
#define SELECTED_DOUBLE_COUNT 100

#define DEST_ADDR "172.20.20.3"

#define EVENTS_COUNT_PAPI 2
int EVENTS_PAPI[EVENTS_COUNT_PAPI] = {PAPI_TOT_CYC, PAPI_L3_TCM};
char EVENTS_STRING[EVENTS_COUNT_PAPI][100] = {"Total CPU Cycles", "L3 Cache Misses"};

std::unordered_map<int, char*> str_map;
std::unordered_map<int, int*> int_map;
std::unordered_map<int, double*> double_map;

// Function to generate a random string of a given length


static void generateRandomString(size_t length, char* random_string) {
	char characters[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
	for (size_t i = 0; i < length-1; ++i) {
	    random_string[i] = characters[rand() % strlen(characters)];
	}
	random_string[length-1] = '\0';
}
// Structure to hold pre-generated strings
struct StringCache {
    std::unordered_map<size_t, std::vector<char*>> size_to_strings;
    
    ~StringCache() {
        // Cleanup
        for (auto& pair : size_to_strings) {
            for (char* str : pair.second) {
                delete[] str;
            }
        }
    }
    
    bool loadStrings(const std::string& dir_path, size_t required_size_mb) {
        std::string filename = dir_path + "/strings_" + std::to_string(required_size_mb) + "MB.bin";
        // std::cout << "Attempting to load strings from: " << filename << std::endl;
        if(size_to_strings.find(required_size_mb)!=size_to_strings.end()) return true;
        std::ifstream infile(filename, std::ios::binary);
        if (!infile) {
            std::cerr << "Failed to open file: " << filename << std::endl;
            std::cerr << "Error: " << strerror(errno) << std::endl;
            return false;
        }
        
        // Read header
        size_t size_bytes;
        int num_strings;
        
        infile.read(reinterpret_cast<char*>(&size_bytes), sizeof(size_bytes));
        if (!infile) {
            std::cerr << "Failed to read size_bytes from header" << std::endl;
            return false;
        }
        
        infile.read(reinterpret_cast<char*>(&num_strings), sizeof(num_strings));
        if (!infile) {
            std::cerr << "Failed to read num_strings from header" << std::endl;
            return false;
        }
        
        // std::cout << "File header: size_bytes=" << size_bytes << ", num_strings=" << num_strings << std::endl;
        
        // Read strings
        std::vector<char*>& strings = size_to_strings[required_size_mb];
        for (int i = 0; i < num_strings; ++i) {
            try {
                char* buffer = new char[size_bytes];
                infile.read(buffer, size_bytes);
                if (!infile) {
                    std::cerr << "Failed to read string " << i << std::endl;
                    delete[] buffer;
                    return false;
                }
                strings.push_back(buffer);
                // if (i % 100 == 0) {
                //     // std::cout << "Loaded " << i << " strings\r" << std::flush;
                // }
            } catch (const std::bad_alloc& e) {
                std::cerr << "Memory allocation failed for string " << i << ": " << e.what() << std::endl;
                return false;
            }
        }
        // std::string filename = dir_path + "/strings_" + std::to_string(size_mb) + "MB.bin";
    
        // int fd = open(filename.c_str(), O_RDONLY);
        // if (fd == -1) {
        //     std::cerr << "Failed to open file: " << strerror(errno) << std::endl;
        //     return false;
        // }
        
        // struct stat sb;
        // if (fstat(fd, &sb) == -1) {
        //     std::cerr << "Failed to get file size: " << strerror(errno) << std::endl;
        //     close(fd);
        //     return false;
        // }
        
        // void* mapped = mmap(NULL, sb.st_size, PROT_READ, MAP_SHARED, fd, 0);
        // close(fd);
        
        // if (mapped == MAP_FAILED) {
        //     std::cerr << "Failed to mmap: " << strerror(errno) << std::endl;
        //     return false;
        // }
        
        // char* data = static_cast<char*>(mapped);
        // size_t size_bytes;
        // int num_strings;
        
        // memcpy(&size_bytes, data, sizeof(size_bytes));
        // memcpy(&num_strings, data + sizeof(size_bytes), sizeof(num_strings));
        
        // char* strings_start = data + sizeof(size_bytes) + sizeof(num_strings);
        
        // std::vector<char*>& strings = size_to_strings[required_size_mb];
        // for (int i = 0; i < num_strings; i++) {
        //     strings.push_back(strings_start + i * size_bytes);
        // }
        // std::cout << "\nSuccessfully loaded " << strings.size() << " strings" << std::endl;
        return true;
    }
    void printCacheStatus() {
        std::cout << "String Cache Status:" << std::endl;
        for (const auto& pair : size_to_strings) {
            std::cout << "Size " << pair.first << "MB: " 
                        << pair.second.size() << " strings loaded" << std::endl;
        }
    }
    char* getString(size_t size_mb, int index) {
        // std::cout << "getString called with size_mb=" << size_mb << std::endl;
        // std::cout << "Available sizes in cache: ";
        // for (const auto& pair : size_to_strings) {
        //     std::cout << pair.first << " ";
        // }
        // std::cout << std::endl;

        auto it = size_to_strings.find(size_mb);
        if (it == size_to_strings.end()) {
            std::cerr << "No exact match found for size " << size_mb << "MB" << std::endl;
            return nullptr;
        }

        if (index >= it->second.size()) {
            std::cerr << "Index " << index << " out of bounds. Size is " << it->second.size() << std::endl;
            return nullptr;
        }

        return it->second[index];
    }
    // char* getString(size_t size_mb, int index) {
    //     auto it = size_to_strings.find(size_mb);
    //     if (it == size_to_strings.end() || index >= it->second.size()) {
    //         std::cout<<"NULL field"<<std::endl;
    //         return nullptr;
    //     }
    //     return it->second[index];
    // }
};

// Global string cache
static StringCache STRING_CACHE;

// Function to initialize strings for a specific size
bool initialize_strings_for_size(const std::string& data_dir, size_t size_mb) {
    // Check if already loaded
    if (STRING_CACHE.size_to_strings.find(size_mb) != STRING_CACHE.size_to_strings.end()) {
        return true;
    }
    
    return STRING_CACHE.loadStrings(data_dir, size_mb);
}

// Modified initialise_strings function for the client files
bool initialise_strings(const std::string& data_dir, size_t size_mb) {
    if (!STRING_CACHE.loadStrings(data_dir, size_mb)) {
        std::cerr << "Failed to load strings from " << data_dir << " of size " << size_mb << "MB" << std::endl;
        return false;
    }
    
    // Fill str_map with pointers to pre-generated strings
    for (size_t i = 1; i <= STRING_COUNT; ++i) {
        // std::cout<<size_mb<<std::endl;
        str_map[i] = STRING_CACHE.getString(size_mb, i % 1000); // Reuse strings if we need more than 1000
        if (!str_map[i]) {
            std::cerr << "Failed to get string " << i << " from cache" << std::endl;
            return false;
        }
    }
    // std::cout << "Successfully loaded all strings into str_map" << std::endl;
    // STRING_CACHE.printCacheStatus();
    return true;
}
//class StringStore{


//private:
//	static bool init;
//	StringStore() {};
//	static void initialise_strings2(int size) {
//	  srand(static_cast<unsigned int>(time(0)));
//	  for (size_t i = 1; i <= STRING_COUNT; ++i) {
//	    char* str = new char[size];
//	    generateRandomString(size, str);
//	    str_map[i] = str;
//	  }
//	}
	

//public:
//	static std::unordered_map<int, char*> str_map;
//	static void initialize(int size){
//		if(!init){
//			initialise_strings2(size);
//			init = true;
//		}
//	}
//};

//std::unordered_map<int, char*> StringStore::str_map;
//bool StringStore::init = false;

void initialise_integers() {
  srand(static_cast<unsigned int>(time(0)));
  for (size_t i = 1; i <= INT_COUNT; ++i) {
    int x = rand() % INT32_MAX;
    int_map[i] = new int(x);
  }
}

void initialise_doubles() {
  srand(static_cast<unsigned int>(time(0)));
  for (size_t i = 1; i <= INT_COUNT; ++i) {
    double x = static_cast<double>(rand()) / static_cast<double>(RAND_MAX); // Random double between 0 and 1
    double_map[i] = new double(x);
  }
}

#endif // COMMON_SEPARATED
