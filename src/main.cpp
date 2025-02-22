#include "Logger.hpp"
#include "Server.hpp"
#include <cstring>
#include <iostream>
#include <signal.h>
#include <sys/socket.h>
#include <unistd.h>

void handle_sigpipe(int sig) {
  std::cerr << "SIGPIPE caught: " << strsignal(sig) << std::endl;
}
int main() {
  struct sigaction sa;
  sa.sa_handler = handle_sigpipe;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  sigaction(SIGPIPE, &sa, nullptr);
  Server server(8080);
  Logger::getInstance().setLogFile("mustore.log");
  Logger::getInstance().setLogLevel(LogLevel::DEBUG);
  LOG_INFO("Starting server on port 8080");
  server.start();
  return 0;
}
