// Server.cpp
#include "Server.hpp"
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <thread>
#include <sstream>
#include <cstring>

Server::Server(int port) : port(port) {
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);
}

void Server::start() {
    bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    listen(serverSocket, 5);
    
    while (true) {
        int clientSocket = accept(serverSocket, nullptr, nullptr);
        std::thread(&Server::handleClient, this, clientSocket).detach();
    }
}

void Server::handleClient(int clientSocket) {

    while (true) {
        std::string data;
        char buffer[1024];
        while (true) {
            int bytesRead = read(clientSocket, buffer, sizeof(buffer));
            if (bytesRead <= 0) goto end;
            data.append(buffer, bytesRead);
            if (bytesRead < sizeof(buffer)) break; 
        }        
        processCommand(data, clientSocket);
        memset(buffer, 0, 1024);
    }
end:
    close(clientSocket);
}

void Server::processCommand(const std::string& command, int clientSocket) {
    std::istringstream iss(command);
    std::string op, key, value;
    iss >> op;
    
}