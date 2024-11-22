#include "Server.hpp"
#include <iostream>

int main() {
    Server server(8080);
    std::cout << "Starting KV store server on port 8080..." << std::endl;
    server.start();
    return 0;
}