#include "Server.hpp"
#include <iostream>
#include "Logger.hpp"

int main() {
    Server server(8080);
    Logger::getInstance().setLogFile("mustore.log");
    Logger::getInstance().setLogLevel(LogLevel::DEBUG);
    LOG_INFO("Starting server on port 8080");
    server.start();
    return 0;
}