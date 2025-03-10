// #include "common.hpp"
// #include "message.hpp"
// #include "net/net.hpp"
// #include "define.hpp"
// #include <chrono>
// #include <thread>

// int SELECTED_STRING_COUNT;  // Total number of strings
// int STRING_CHAR_COUNT;      // Size of each string
// int iter;
// const int NUM_FIELDS = 8;   // Number of repeated string fields

// void test_serialization(const std::string& data_dir, user::SerializationMethod method) {
//     // Create TCP connection
//     user::net::Address server_addr("172.20.20.3", 12345);
//     user::net::TCPClient client(server_addr);
    
//     if (!client.Connect()) {
//         std::cerr << "Failed to connect to server" << std::endl;
//         return;
//     }

//     // Create a message using the TestMessage proto definition
//     std::string msg_name = "TestMessage";
//     auto message = user::InitMessage::allocateMessage(msg_name);

//     // Calculate strings per field (distribute evenly across 8 fields)
//     int strings_per_field = SELECTED_STRING_COUNT / NUM_FIELDS;
//     int remaining_strings = SELECTED_STRING_COUNT % NUM_FIELDS;

//     // Add strings to each field
//     for (int field_num = 1; field_num <= NUM_FIELDS; field_num++) {
//         std::string field_name = "data" + std::to_string(field_num);
        
//         // Calculate how many strings this field should get
//         int strings_this_field = strings_per_field;
//         if (remaining_strings > 0) {
//             strings_this_field++;
//             remaining_strings--;
//         }

//         // Add strings to this field
//         for (int i = 0; i < strings_this_field; i++) {
//             // Calculate the global string index
//             int global_idx = ((field_num - 1) * strings_per_field) + i;
//             char* str = STRING_CACHE.getString(STRING_CHAR_COUNT, global_idx % 1000);
//             if (str) {
//                 message->addVal(field_name, str);
//             }
//         }
//     }

//     // Sleep to allow ftrace to start
//     sleep(0.5);
//     // std::this_thread::sleep_for(std::chrono::milliseconds(250));

//     // Serialize and send with specified method
//     auto start = std::chrono::high_resolution_clock::now();
//     ssize_t bytes = message->serializeAndWrite(&client, method);
//     auto end = std::chrono::high_resolution_clock::now();

//     auto c = end-start;
//     std::cout<<c.count()<<std::endl;
//     if (bytes == -1) {
//         std::cerr << "Failed to serialize and send message" << std::endl;
//     }

//     // Clean up
//     delete message;
//     client.Close();
// }

// int main(int argc, char const *argv[]) {
//     if (argc != 7) {
//         std::cerr << "Usage: " << argv[0] << " <total_size_mb> <field_count> <iteration> <data_dir> <method> <proto_file>" << std::endl;
//         return 1;
//     }

//     // First argument is now total size in MB
//     int total_size_mb = atoi(argv[1]);
    
//     // Calculate individual string sizes
//     STRING_CHAR_COUNT = (total_size_mb * 1024 * 1024) / NUM_FIELDS;  // Divide total size by number of fields
//     SELECTED_STRING_COUNT = NUM_FIELDS;  // One string per field
    
//     iter = atoi(argv[3]);
//     std::string data_dir = argv[4];
//     std::string method_str = argv[5];
//     std::string proto_file = argv[6];

//     // Initialize SymTab and read proto file
//     user::SymTab::init();
//     user::SymTab::readFile(proto_file);

//     std::cout << "Total size: " << total_size_mb << "MB" << std::endl;
//     std::cout << "Size per field: " << (total_size_mb / NUM_FIELDS) << "MB" << std::endl;

//     // Initialize strings using the total size divided by number of fields
//     // if (!initialise_strings(data_dir, total_size_mb / NUM_FIELDS)) {
//     //     std::cerr << "Failed to initialize strings of size " << (total_size_mb / NUM_FIELDS) << "MB" << std::endl;
//     //     return 1;
//     // }

//     // Determine serialization method
//     user::SerializationMethod method;
//     if (method_str == "COPY") {
//         method = user::SerializationMethod::COPY;
//     } else if (method_str == "SG") {
//         method = user::SerializationMethod::SG;
//     } else if (method_str == "ZC") {
//         method = user::SerializationMethod::ZC;
//     } else {
//         std::cerr << "Invalid method. Use COPY, SG, or ZC" << std::endl;
//         return 1;
//     }

//     test_serialization(data_dir, method);
//     return 0;
// }
#include "common.hpp"
#include "message.hpp"
#include "net/net.hpp"
#include "define.hpp"
#include "symtab.hpp"
#include <chrono>
#include <thread>
#include <cmath>

int SELECTED_STRING_COUNT;  // Total number of strings
int STRING_CHAR_COUNT;      // Size of each string
int iter;
const int NUM_FIELDS = 8;   // Number of repeated string fields

void test_serialization(const std::string& data_dir, user::SerializationMethod method) {
    // Create TCP connection
    user::net::Address server_addr("172.20.20.3", 12345);
    user::net::TCPClient client(server_addr);
    
    if (!client.Connect()) {
        std::cerr << "Failed to connect to server" << std::endl;
        return;
    }

    // Create a message using the TestMessage proto definition
    std::string msg_name = "TestMessage";
    auto message = user::InitMessage::allocateMessage(msg_name);
    // std::cout << "Created message with name: " << msg_name << std::endl;

    // Add strings to each field
    // for (int field_num = 1; field_num <= NUM_FIELDS; field_num++) {
    //     std::string field_name = "data" + std::to_string(field_num);
    //     char* str = STRING_CACHE.getString(STRING_CHAR_COUNT, field_num % 1000);
    //     if (str) {
    //         message->setVal(field_name, str);
    //         std::cout << "Added value to field: " << field_name << std::endl;
    //     } else {
    //         std::cerr << "Failed to get string for field " << field_num << std::endl;
    //         delete message;
    //         client.Close();
    //         return;
    //     }
    // }

    // Add strings to each field
    for (int field_num = 1; field_num <= NUM_FIELDS; field_num++) {
        std::string field_name = "data" + std::to_string(field_num);
        // Use str_map directly
        if (str_map.find(field_num) == str_map.end()) {
            std::cerr << "Failed to find string " << field_num << " in str_map" << std::endl;
            delete message;
            client.Close();
            return;
        }
        message->addVal(field_name, str_map[field_num]);
        // std::cout << "Added value to field: " << field_name << std::endl;
    }
    // Sleep to allow ftrace to start
    // std::this_thread::sleep_for(std::chrono::milliseconds(250));
    // std::cout << "About to serialize and send message with method: " 
    //           << user::SerializationMethodToStr(method) << std::endl;
    // Serialize and send with specified method
    auto start = std::chrono::high_resolution_clock::now();
    ssize_t bytes = message->serializeAndWrite(&client, method);
    auto end = std::chrono::high_resolution_clock::now();
    std::cout<<(end-start).count()<<std::endl;
    if (bytes == -1) {
        std::cerr << "Failed to serialize and send message" << std::endl;
    } else {
        // std::cout << "Successfully sent " << bytes << " bytes" << std::endl;
    }

    // Clean up
    delete message;
    client.Close();
}

int main(int argc, char const *argv[]) {
    LatencyRecorderMgr::Init();
    if (argc != 6) {  // Changed from 7 to 6
        std::cerr << "Usage: " << argv[0] << " <total_size_mb> <iteration> <data_dir> <method> <proto_file>" << std::endl;
        return 1;
    }
    // First argument is now total size in MB
    double total_size_mb = atof(argv[1]);
    
    // Calculate individual string sizes (use floating point for precision)
    double size_per_field_mb = total_size_mb / NUM_FIELDS;
    STRING_CHAR_COUNT = static_cast<int>(std::ceil(size_per_field_mb * 1024 * 1024));
    SELECTED_STRING_COUNT = NUM_FIELDS;

    // Round up STRING_CHAR_COUNT to the nearest MB to match our pre-generated files
    size_t rounded_size_mb = (STRING_CHAR_COUNT + (1024 * 1024) - 1) / (1024 * 1024);
    size_t total_bytes = static_cast<size_t>(total_size_mb * 1024 * 1024);
    
    iter = atoi(argv[2]);
    std::string data_dir = argv[3];
    std::string method_str = argv[4];
    std::string proto_file = argv[5];
    LatencyRecorderMgr::Get()->StartNewStats(
        0,  // type
        method_str == "COPY" ? serialization_method_t::copy : 
        method_str == "SG" ? serialization_method_t::sg : 
        serialization_method_t::zc,
        total_bytes,  // total size in bytes
        NUM_FIELDS
    );
    // Initialize SymTab and read proto file
    user::SymTab::init();
    user::SymTab::readFile(proto_file);
    
    // Verify the symbol table has our message
    auto symbol = user::SYMTAB->getSymbol("TestMessage");
    // if (!symbol) {
    //     std::cerr << "Failed to find TestMessage in symbol table" << std::endl;
    //     return 1;
    // }
    // std::cout << "Found TestMessage in symbol table with " << symbol->members_.size() << " members" << std::endl;
    // for (const auto& member : symbol->members_) {
    //     std::cout << "Member: " << member.name_ << " ID: " << member.id_ << " Repeated: " << member.repeated_ << std::endl;
    // }

    // std::cout << "Total size requested: " << total_size_mb << "MB" << std::endl;
    // std::cout << "Size per field (before rounding): " << size_per_field_mb << "MB" << std::endl;
    // std::cout << "Using pre-generated strings of size: " << rounded_size_mb << "MB" << std::endl;

    // Initialize strings using the rounded size
    if (!initialise_strings(data_dir, rounded_size_mb)) {
        std::cerr << "Failed to initialize strings of size " << rounded_size_mb << "MB" << std::endl;
        return 1;
    }

    // Determine serialization method
    user::SerializationMethod method;
    if (method_str == "COPY") {
        method = user::SerializationMethod::COPY;
    } else if (method_str == "SG") {
        method = user::SerializationMethod::SG;
    } else if (method_str == "ZC") {
        method = user::SerializationMethod::ZC;
    } else {
        std::cerr << "Invalid method. Use COPY, SG, or ZC" << std::endl;
        return 1;
    }

    test_serialization(data_dir, method);
    return 0;
}