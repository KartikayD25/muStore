#include "epoll_instance.hpp"
#include "epoll_server.hpp"

#include <iostream>

using namespace std;
using common::port_t;

#define BACKLOG 5
port_t port = 8081;

Address server_add = Address("127.0.0.1", port);

void Server() {
  EpollServer es(server_add);
  // es.Serve();
}

void Client() {
  size_t buf_size = 102400;
  char buf[buf_size] = {'\0'};
  Address server_address = Address("10.0.1.1", port);
  TCPClient client(server_address);
  assert(client.Connect());
  // client.Write("Hello", 5);

  std::cout << "Read: " << (client.Read(buf, buf_size)) << std::endl;
  //  client.Close();
  std::cout << "Closed\n";
}

int main(int argc, char *argv[]) {
  int option;
  int is_server = 0;
  string server_addr = "";
  string interface = "";
  while ((option = getopt(argc, argv, "sc:i:h")) != -1) {
    switch (option) {
    case 's':
      is_server = 1;
      break;
    case 'c':
      is_server = 0;
      server_addr = optarg;
      break;
    case 'i':
      interface = optarg;
      break;
    case 'h':
    default:
      printf("Help/Usage Example\n");
      abort();
    }
  }

  if (is_server) {
    Server();
  } else {
    Client();
  }
  return 0;
}

// int main() { return 0; }
