#include "lockless_ringBuffer.hpp"
#include "thread_pool.hpp"
#include "string_pool.hpp"
#include "message.hpp"
#include "net/net.hpp"
#include "symtab.hpp"
#include <iostream>
#include <chrono>
#include <thread>
#include <atomic>
#include <netinet/tcp.h>
#include <signal.h>

// Structure to represent a client request
struct Request {
    int client_socket;
    uint32_t message_size_mb;
    
    Request() : client_socket(-1), message_size_mb(0) {}
    Request(int sock, uint32_t size) : client_socket(sock), message_size_mb(size) {}
};

// Server class to handle client connections and process requests
class Server {
private:
    // Configuration
    user::SerializationMethod serialization_method_;
    std::string data_dir_;
    
    // Network
    int server_socket_;
    bool running_;
    std::atomic<bool> initialized_;
    
    // Worker thread management
    ThreadPool thread_pool_;
    LocklessRingBuffer<Request> request_queue_;
    std::thread consumer_thread_;
    
    // Data
    StringPool string_pool_;
    
    // Process requests from the queue
    void processRequests() {
        while (running_) {
            Request req;
            if (request_queue_.tryDequeue(req)) {
                // Submit the request to the thread pool
                thread_pool_.enqueue([this, req]() {
                    processRequest(req);
                });
            } else {
                // Wait a bit before checking again
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        }
    }
    
    // Process a single request
    void processRequest(const Request& req) {
        try {
            // Read the message size
            uint32_t size_mb = req.message_size_mb;
            
            // Create socket wrapper for the client
            user::net::Conn client(user::net::ConnProto::TCP, req.client_socket);
            
            // Determine string size to use (size per field)
            size_t field_size_mb;
            if (size_mb == 8) {
                field_size_mb = 1;  // Use 1MB strings for 8MB message
            } else if (size_mb == 40) {
                field_size_mb = 5;  // Use 5MB strings for 40MB message
            } else if (size_mb == 80) {
                field_size_mb = 10; // Use 10MB strings for 80MB message
            } else {
                // Invalid size - send error response
                std::cerr << "Invalid message size requested: " << size_mb << "MB" << std::endl;
                const char* error_msg = "Invalid size";
                client.Write(error_msg, strlen(error_msg));
                return;
            }
            
            // Create message
            std::string msg_name = "TestMessage";
            auto message = user::InitMessage::allocateMessage(msg_name);
            
            // Add strings to each field
            for (int field_num = 1; field_num <= 8; field_num++) {
                std::string field_name = "data" + std::to_string(field_num);
                char* str = string_pool_.getString(field_size_mb, field_num % 100);
                if (str) {
                    message->addVal(field_name, str);
                } else {
                    std::cerr << "Failed to get string for field " << field_num << std::endl;
                    const char* error_msg = "String retrieval failed";
                    client.Write(error_msg, strlen(error_msg));
                    delete message;
                    return;
                }
            }
            
            // Serialize and send message
            ssize_t bytes = message->serializeAndWrite(&client, serialization_method_);
            
            // Clean up
            delete message;
            
            std::cout << "Sent response of size " << bytes << " bytes" << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Error processing request: " << e.what() << std::endl;
        }
    }
    
    // Setup socket with optimizations
    bool setupSocket() {
        // Create socket
        server_socket_ = socket(AF_INET, SOCK_STREAM, 0);
        if (server_socket_ == -1) {
            std::cerr << "Failed to create socket: " << strerror(errno) << std::endl;
            return false;
        }
        
        // Set socket options
        int opt = 1;
        if (setsockopt(server_socket_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
            std::cerr << "Failed to set SO_REUSEADDR: " << strerror(errno) << std::endl;
            close(server_socket_);
            return false;
        }
        
        if(setsockopt(server_socket_, SOL_SOCKET, SO_ZEROCOPY, &opt, sizeof(opt)) == -1){
            std::cerr << "Failed to set SO_ZEROCOPY: " << strerror(errno) << std::endl;
            close(server_socket_);
            return false;
        }

        // Set TCP_NODELAY
        if (setsockopt(server_socket_, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt)) == -1) {
            std::cerr << "Failed to set TCP_NODELAY: " << strerror(errno) << std::endl;
            close(server_socket_);
            return false;
        }
        
        // int clear = 0;
        // setsockopt(server_socket_, SOL_SOCKET, SO_SNDBUF, &clear, sizeof(clear));

        // Set large buffer sizes
        int buffer_size = 209715200;  // 200MB
        if (setsockopt(server_socket_, SOL_SOCKET, SO_RCVBUF, &buffer_size, sizeof(buffer_size)) == -1) {
            std::cerr << "Failed to set SO_RCVBUF: " << strerror(errno) << std::endl;
            // Not fatal, continue
        }
        
        if (setsockopt(server_socket_, SOL_SOCKET, SO_SNDBUF, &buffer_size, sizeof(buffer_size)) == -1) {
            std::cerr << "Failed to set SO_SNDBUF: " << strerror(errno) << std::endl;
            // Not fatal, continue
        }
        
        // Bind to address
        struct sockaddr_in server_addr;
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(12345);
        server_addr.sin_addr.s_addr = INADDR_ANY;
        
        if (bind(server_socket_, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
            std::cerr << "Failed to bind: " << strerror(errno) << std::endl;
            close(server_socket_);
            return false;
        }
        
        // Listen
        if (listen(server_socket_, 1024) == -1) {
            std::cerr << "Failed to listen: " << strerror(errno) << std::endl;
            close(server_socket_);
            return false;
        }
        
        return true;
    }
    
public:
    Server(const std::string& data_dir, user::SerializationMethod method, int num_threads = 8)
        : serialization_method_(method),
          data_dir_(data_dir),
          server_socket_(-1),
          running_(false),
          initialized_(false),
          thread_pool_(num_threads),
          request_queue_(1024) {
    }
    
    ~Server() {
        shutdown();
    }
    
    // Initialize the server
    bool initialize() {
        if (initialized_) {
            return true;
        }
        
        // Initialize symbol table
        user::SymTab::init();
        user::SymTab::readFile("test_message.proto");
        
        // Initialize string pool
        if (!string_pool_.initialize(data_dir_)) {
            std::cerr << "Failed to initialize string pool" << std::endl;
            return false;
        }
        
        // Setup network socket
        if (!setupSocket()) {
            return false;
        }
        
        initialized_ = true;
        return true;
    }
    
    // Start the server
    bool start() {
        if (!initialized_ && !initialize()) {
            return false;
        }
        
        running_ = true;
        
        // Start consumer thread
        consumer_thread_ = std::thread(&Server::processRequests, this);
        
        std::cout << "Server started. Waiting for connections..." << std::endl;
        
        // Accept connections
        while (running_) {
            struct sockaddr_in client_addr;
            socklen_t client_addr_len = sizeof(client_addr);
            
            int client_socket = accept(server_socket_, (struct sockaddr*)&client_addr, &client_addr_len);
            if (client_socket == -1) {
                if (errno == EINTR) {
                    // Interrupted, check if we should continue
                    continue;
                }
                std::cerr << "Failed to accept: " << strerror(errno) << std::endl;
                break;
            }
            
            // Set large buffer sizes for client socket
            int buffer_size = 104857600;  // 100MB
            if (setsockopt(client_socket, SOL_SOCKET, SO_RCVBUF, &buffer_size, sizeof(buffer_size)) == -1) {
                std::cerr << "Failed to set client SO_RCVBUF: " << strerror(errno) << std::endl;
                // Not fatal, continue
            }
            
            if (setsockopt(client_socket, SOL_SOCKET, SO_SNDBUF, &buffer_size, sizeof(buffer_size)) == -1) {
                std::cerr << "Failed to set client SO_SNDBUF: " << strerror(errno) << std::endl;
                // Not fatal, continue
            }
            
            // Handle the client connection
            std::cout << "Client connected" << std::endl;
            handleClient(client_socket);
        }
        
        return true;
    }
    
    // Handle a client connection
    void handleClient(int client_socket) {
        // Create a new thread for this client
        std::thread([this, client_socket]() {
            try {
                while (running_) {
                    // Read request message size
                    uint32_t size_mb;
                    ssize_t bytes_read = recv(client_socket, &size_mb, sizeof(size_mb), 0);
                    
                    if (bytes_read <= 0) {
                        // Client disconnected or error
                        if (bytes_read < 0) {
                            std::cerr << "Error reading from client: " << strerror(errno) << std::endl;
                        }
                        break;
                    }
                    
                    std::cout << "Received request for " << size_mb << "MB message" << std::endl;
                    
                    // Add request to queue
                    Request req(client_socket, size_mb);
                    if (!request_queue_.tryEnqueue(req)) {
                        std::cerr << "Request queue full, dropping request" << std::endl;
                        // Send error response
                        const char* error_msg = "Server busy";
                        send(client_socket, error_msg, strlen(error_msg), 0);
                    }
                }
            } catch (const std::exception& e) {
                std::cerr << "Error handling client: " << e.what() << std::endl;
            }
            
            // Close client socket
            close(client_socket);
            std::cout << "Client disconnected" << std::endl;
        }).detach();
    }
    
    // Shutdown the server
    void shutdown() {
        running_ = false;
        
        if (consumer_thread_.joinable()) {
            consumer_thread_.join();
        }
        
        if (server_socket_ != -1) {
            close(server_socket_);
            server_socket_ = -1;
        }
        
        initialized_ = false;
    }
};

// Signal handler for clean shutdown
std::atomic<bool> stop_server(false);
void signalHandler(int signum) {
    std::cout << "Interrupt signal (" << signum << ") received. Shutting down..." << std::endl;
    stop_server.store(true);
}

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <data_dir> <serialization_method>" << std::endl;
        std::cerr << "  serialization_method: COPY, SG, or ZC" << std::endl;
        return 1;
    }
    
    std::string data_dir = argv[1];
    std::string method_str = argv[2];
    
    // Convert method string to enum
    user::SerializationMethod method;
    if (method_str == "COPY") {
        method = user::SerializationMethod::COPY;
    } else if (method_str == "SG") {
        method = user::SerializationMethod::SG;
    } else if (method_str == "ZC") {
        method = user::SerializationMethod::ZC;
    } else {
        std::cerr << "Invalid serialization method. Use COPY, SG, or ZC" << std::endl;
        return 1;
    }
    
    // // Register signal handler
    // signal(SIGINT, signalHandler);
    
    // Create and start server
    Server server(data_dir, method);
    
    if (!server.initialize()) {
        std::cerr << "Failed to initialize server" << std::endl;
        return 1;
    }
    
    std::thread server_thread([&server]() {
        server.start();
    });
    
    // Wait for stop signal
    while (!stop_server.load()) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    
    // Shutdown server
    server.shutdown();
    
    if (server_thread.joinable()) {
        server_thread.join();
    }
    
    return 0;
}