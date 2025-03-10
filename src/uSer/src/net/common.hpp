#pragma once

#include <arpa/inet.h>
#include <cassert>
#include <cstring>
#include <errno.h>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <x86intrin.h>

#define NOT_IMPLEMENTED                                                        \
  std::cout << __FILE__ << ":" << __LINE__ << "\t" << __FUNCTION__             \
            << " NOT_IMPLEMENTED " << std::endl;                               \
  abort();

namespace user {
namespace net {
namespace common {

size_t BUFFER_SIZE = 2048; // 10268; // 2116;//1024;

typedef uint16_t port_t;

struct Address {
  const std::string ip_;
  const port_t port_;

  Address() : ip_("0.0.0.0"), port_(0) {}
  Address(const std::string ip, const port_t port) : ip_(ip), port_(port) {}
  Address(const Address &addr) : ip_(addr.ip_), port_(addr.port_) {}
  friend std::ostream &operator<<(std::ostream &o, Address const &addr) {
    o << addr.ip_ << ":" << addr.port_;
    return o;
  }
};

inline void HandleSyscallError(const std::string syscall_name) {
  fprintf(stderr, "%s %d ", syscall_name.c_str(), errno);
  // perror(syscall_name.c_str());
  perror("");
  abort();
}

void SetIPV4Addr(struct sockaddr_in *ipv4_addr, const char *ip,
                 const port_t port) {
  ipv4_addr->sin_family = AF_INET;
  ipv4_addr->sin_addr.s_addr = inet_addr(ip);
  ipv4_addr->sin_port = htons(port);
}

void BindToInterface(int sock, const char *interface) {
  if (interface && strlen(interface) > 0 &&
      (setsockopt(sock, SOL_SOCKET, SO_BINDTODEVICE, interface,
                  strlen(interface)) != 0)) {
    HandleSyscallError("setsockopt:SO_BINDTODEVICE");
  }
}

void ReUseAddress(int sock, int *sock_opt) {
  if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &sock_opt,
                 sizeof(*sock_opt)) == -1) {
    HandleSyscallError("setsockopt-reuse");
  }
}

void BindThisThread(int core_id) {
  int num_cores = sysconf(_SC_NPROCESSORS_ONLN);
  assert(!(core_id < 0 || core_id >= num_cores));

  cpu_set_t cpuset;
  CPU_ZERO(&cpuset);
  CPU_SET(core_id, &cpuset);

  pthread_t current_thread = pthread_self();
  assert(pthread_setaffinity_np(current_thread, sizeof(cpu_set_t), &cpuset) ==
         0);
}

void FlushFromCache(void *var) {
  _mm_mfence();
  _mm_clflush(var);
  _mm_mfence();
}

} // namespace common
} // namespace net
} // namespace user
