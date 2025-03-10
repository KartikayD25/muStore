#pragma once

#include <cstring>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <unordered_map>
#include <vector>

class StringPool {
private:
  // Store mapped memory regions to avoid memory leaks
  struct MappedRegion {
    void *data;
    size_t size;

    MappedRegion(void *d, size_t s) : data(d), size(s) {}
    ~MappedRegion() {
      if (data) {
        munmap(data, size);
      }
    }
  };

  std::unordered_map<size_t, std::vector<char *>> size_to_strings;
  std::vector<std::shared_ptr<MappedRegion>> mapped_regions;

public:
  ~StringPool() {
    // MappedRegion destructors will handle unmapping
  }

  // Initialize by loading strings of different sizes
  bool initialize(const std::string &dir_path) {
    // Load strings of sizes 1MB, 5MB, and 10MB
    return loadStrings(dir_path, 1) && loadStrings(dir_path, 5) &&
           loadStrings(dir_path, 10);
  }

  // Load strings from file for a specific size
  bool loadStrings(const std::string &dir_path, size_t size_mb) {
    std::string filename =
        dir_path + "/strings_" + std::to_string(size_mb) + "MB.bin";

    int fd = open(filename.c_str(), O_RDONLY);
    if (fd == -1) {
      std::cerr << "Failed to open file: " << filename << " - "
                << strerror(errno) << std::endl;
      return false;
    }

    struct stat sb;
    if (fstat(fd, &sb) == -1) {
      std::cerr << "Failed to get file size: " << strerror(errno) << std::endl;
      close(fd);
      return false;
    }

    void *mapped = mmap(NULL, sb.st_size, PROT_READ, MAP_SHARED, fd, 0);
    close(fd);

    if (mapped == MAP_FAILED) {
      std::cerr << "Failed to mmap: " << strerror(errno) << std::endl;
      return false;
    }

    // Store mapped region
    auto region = std::make_shared<MappedRegion>(mapped, sb.st_size);
    mapped_regions.push_back(region);

    char *data = static_cast<char *>(mapped);
    size_t size_bytes;
    int num_strings;

    memcpy(&size_bytes, data, sizeof(size_bytes));
    memcpy(&num_strings, data + sizeof(size_bytes), sizeof(num_strings));

    char *strings_start = data + sizeof(size_bytes) + sizeof(num_strings);

    std::vector<char *> &strings = size_to_strings[size_mb];

    // Load only 100 strings (or all if less than 100)
    int strings_to_load = std::min(100, num_strings);
    for (int i = 0; i < strings_to_load; i++) {
      strings.push_back(strings_start + i * size_bytes);
    }

    std::cout << "Loaded " << strings.size() << " strings of size " << size_mb
              << "MB" << std::endl;
    return true;
  }

  // Get a string for a specific size
  char *getString(size_t size_mb, int index) {
    auto it = size_to_strings.find(size_mb);
    if (it == size_to_strings.end() || it->second.empty()) {
      return nullptr;
    }
    return it->second[index % it->second.size()];
  }

  // Get number of strings available for a specific size
  size_t getStringCount(size_t size_mb) {
    auto it = size_to_strings.find(size_mb);
    if (it == size_to_strings.end()) {
      return 0;
    }
    return it->second.size();
  }
};