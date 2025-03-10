#pragma once
#include "core_utils.cpp"
#include <atomic>
#include <map>
#include <unistd.h>
#include <sys/socket.h>
#include "../test/xperiment/Person.hpp"
#include "../net/net.hpp"
#include <vector>
#include <string>



enum class TestMode {
    ONE_TO_ONE,
    MANY_TO_ONE,
    ONE_TO_MANY
};

struct TestConfig {
    TestMode mode;
    std::vector<int> client_cores;
    std::vector<int> server_cores;
    size_t request_count;
    serialization_method_t serialization_method;
};

class Server {
private:
    TestConfig config;
    std::vector<std::thread> worker_threads;
    std::atomic<bool> running{true};
    user::net::Conn * server_conn;
    int server_socket;
    std::map<int, int> socket_to_core_map;  // Maps client sockets to core IDs

    void handle_client(int client_socket, int core_id) {
        CoreAffinity::pin_to_core(core_id);
        
        while(running) {
            // Your serialization library code here for receiving/sending
        }
        close(client_socket);
    }

public:
    Server(const TestConfig& cfg) : config(cfg), running(true) {
        // Create socket
        int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (sock_fd < 0) {
            throw std::runtime_error("Failed to create socket");
        }

        // Set socket options
        int opt = 1;
        if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
            close(sock_fd);
            throw std::runtime_error("Failed to set socket options");
        }

        if(config.serialization_method == serialization_method_t::zc) {
            if(setsockopt(sock_fd, SOL_SOCKET, SO_ZEROCOPY, &opt, sizeof(opt))) {
                close(sock_fd);
                throw std::runtime_error("Failed to set SO_ZEROCOPY flag");
            }
        }
        // Bind socket
        struct sockaddr_in address;
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(8080);  // You might want to make port configurable

        if (bind(sock_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
            close(sock_fd);
            throw std::runtime_error("Failed to bind socket");
        }

        // Listen
        if (listen(sock_fd, SOMAXCONN) < 0) {
            close(sock_fd);
            throw std::runtime_error("Failed to listen on socket");
        }
        
        server_socket = sock_fd;
        server_conn = new user::net::Conn(user::net::ConnProto::TCP,server_socket);
    }

    void start() {
        switch(config.mode) {
            case TestMode::ONE_TO_ONE:
                start_one_to_one();
                break;
            case TestMode::MANY_TO_ONE:
                start_many_to_one();
                break;
            case TestMode::ONE_TO_MANY:
                start_one_to_many();
                break;
        }
    }

private:
    void start_one_to_one() {
        for (size_t i = 0; i < config.server_cores.size(); ++i) {
            worker_threads.emplace_back([this, i]() {
                CoreAffinity::pin_to_core(config.server_cores[i]);
                while (running) {
                    int client_socket = accept(server_socket, nullptr, nullptr);
                    if (client_socket < 0) continue;
                    socket_to_core_map[client_socket] = config.server_cores[i];
                    handle_client(client_socket, config.server_cores[i]);
                }
            });
        }
    }

    void start_many_to_one() {
        worker_threads.emplace_back([this]() {
            CoreAffinity::pin_to_core(config.server_cores[0]);
            while (running) {
                int client_socket = accept(server_socket, nullptr, nullptr);
                if (client_socket < 0) continue;
                handle_client(client_socket, config.server_cores[0]);
            }
        });
    }

    void start_one_to_many() {
        for (int core_id : config.server_cores) {
            worker_threads.emplace_back([this, core_id]() {
                CoreAffinity::pin_to_core(core_id);
                while (running) {
                    int client_socket = accept(server_socket, nullptr, nullptr);
                    if (client_socket < 0) continue;
                    handle_client(client_socket, core_id);
                }
            });
        }
    }
};