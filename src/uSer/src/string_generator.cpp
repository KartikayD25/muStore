#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <random>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstring>
#include <cerrno>

bool createDirectory(const std::string& dir_path) {
    struct stat st = {0};
    if (stat(dir_path.c_str(), &st) == -1) {
        if (mkdir(dir_path.c_str(), 0700) != 0) {
            std::cerr << "Failed to create directory " << dir_path << ": " 
                      << strerror(errno) << std::endl;
            return false;
        }
    }
    return true;
}

void generateRandomString(char* buffer, size_t size) {
    static const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, sizeof(charset) - 2);

    // Generate in smaller chunks to avoid memory issues
    const size_t CHUNK_SIZE = 1024 * 1024; // 1MB chunks
    for (size_t offset = 0; offset < size - 1; offset += CHUNK_SIZE) {
        size_t chunk = std::min(CHUNK_SIZE, size - 1 - offset);
        for (size_t i = 0; i < chunk; ++i) {
            buffer[offset + i] = charset[dis(gen)];
        }
    }
    buffer[size - 1] = '\0';
}

void generateAndSaveStrings(const std::string& dir_path) {
    if (!createDirectory(dir_path)) {
        return;
    }
    
    std::vector<size_t> sizes = {1, 5, 10, 50, 100};
    const int NUM_STRINGS = 1000;
    
    for (size_t size_mb : sizes) {
        std::cout << "\nGenerating " << NUM_STRINGS << " strings of size " << size_mb << "MB" << std::endl;
        size_t size_bytes = size_mb * 1024 * 1024;
        
        std::string filename = dir_path + "/strings_" + std::to_string(size_mb) + "MB.bin";
        std::cout << "Creating file: " << filename << std::endl;
        
        std::ofstream outfile(filename.c_str(), std::ios::binary);
        if (!outfile) {
            std::cerr << "Failed to create file " << filename << ": " 
                      << strerror(errno) << std::endl;
            continue;
        }
        
        std::cout << "Writing header information..." << std::endl;
        outfile.write(reinterpret_cast<const char*>(&size_bytes), sizeof(size_bytes));
        outfile.write(reinterpret_cast<const char*>(&NUM_STRINGS), sizeof(NUM_STRINGS));
        
        if (!outfile) {
            std::cerr << "Failed to write header information" << std::endl;
            outfile.close();
            continue;
        }

        // Generate strings in smaller chunks
        const size_t BUFFER_SIZE = 10 * 1024 * 1024; // 10MB buffer
        size_t num_chunks = (size_bytes + BUFFER_SIZE - 1) / BUFFER_SIZE;
        char* buffer = new char[BUFFER_SIZE];
        
        for (int i = 0; i < NUM_STRINGS; ++i) {
            for (size_t chunk = 0; chunk < num_chunks; ++chunk) {
                size_t chunk_size = std::min(BUFFER_SIZE, size_bytes - chunk * BUFFER_SIZE);
                generateRandomString(buffer, chunk_size);
                outfile.write(buffer, chunk_size);
                
                if (!outfile) {
                    std::cerr << "Failed to write chunk " << chunk << " of string " << i << std::endl;
                    delete[] buffer;
                    outfile.close();
                    return;
                }
            }
            
            if (i % 10 == 0) {
                std::cout << "Generated " << i + 1 << "/" << NUM_STRINGS << " strings\r" << std::flush;
            }
        }
        
        delete[] buffer;
        outfile.close();
        
        if (outfile.fail()) {
            std::cerr << "Error occurred while closing the file" << std::endl;
            continue;
        }
        
        std::cout << "\nSuccessfully generated strings of size " << size_mb << "MB" << std::endl;
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <output_directory>" << std::endl;
        return 1;
    }
    
    std::cout << "Starting string generation process..." << std::endl;
    generateAndSaveStrings(argv[1]);
    std::cout << "String generation complete!" << std::endl;
    return 0;
}