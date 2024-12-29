#pragma once

#include "common.hpp"
#include <cassert>
#include <sys/uio.h>
#include "../message.hpp"
#include <atomic>
#include <functional>
#include <linux/errqueue.h>  // For SO_EE_ORIGIN_ZEROCOPY
#include <sys/socket.h>      // For socket options
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <thread>
namespace user {
  class Message;
namespace net {

using common::Address;
using common::port_t;

enum class ConnProto { TCP, UDP };

struct Conn {
  const enum ConnProto proto_;
  bool is_active_{false}; // Listen/Connect - Close
  int sock_;

  Conn() = delete;
  Conn(enum ConnProto proto) : proto_(proto) {
    if (proto == ConnProto::UDP) {
      NOT_IMPLEMENTED;
    }
    int opt = 1;
    sock_ = socket(AF_INET, SOCK_STREAM,0); // pass SO_ZEROCOPY flag to enable zerocopy.
    setsockopt(sock_,SOL_SOCKET, SO_ZEROCOPY, &opt, sizeof(opt));
    if (sock_ < 0) {
      common::HandleSyscallError("socket");
    }
    completion_thread_ = std::thread(&Conn::processCompletions, this);

  }
  Conn(int sock) : proto_(ConnProto::TCP), is_active_(true), sock_(sock) {
    completion_thread_ = std::thread(&Conn::processCompletions, this);
  };
  ~Conn() { 
    should_stop = true;
    if(completion_thread_.joinable()){
      completion_thread_.join();
    }
    cleanup();  
    Close(); 
  }

  void CloseWrite() {
    if (is_active_ && shutdown(sock_, SHUT_WR) == -1) {
      // common::HandleSyscallError("shutdown");
    }
  }
  void Close() {
    if (is_active_) {
        is_closed_ = true;
        cleanup();
      // common::HandleSyscallError("close");
    }
    is_active_ = false;
  }

  inline int GetSocket() const { return sock_; }
  inline bool IsActive() const { return is_active_; }

  size_t Read(char *buf, const size_t max_size) {
    size_t offset = 0, remaining_size = max_size;
    while (is_active_ && remaining_size) {
      ssize_t bytes = read(sock_, buf + offset, remaining_size);
      // printf("ChunkRead:: Size: %ld Buf: \"%s\"\n", bytes, buf + offset);
      // printf("ChunkRead:: Size: %ld\n", bytes);
      switch (bytes) {
      case -1:
        common::HandleSyscallError("read");
      case 0:
        is_active_ = false;
        break;
      default:
        assert(static_cast<size_t>(bytes) <= remaining_size);
        offset += bytes;
        remaining_size -= bytes;
        break;
      };
    }
    return offset;
  }

  size_t Write(std::string &buf) { return Write(buf.c_str(), buf.length()); }
  size_t Write(const char *buf, const size_t size) {
    ssize_t bytes = write(sock_, buf, size);
    switch (bytes) {
    case -1: {
      common::HandleSyscallError("write");
    }
    case 0: {
      fprintf(stderr, "Write 0 bytes\n");
      common::HandleSyscallError("write");
      is_active_ = false;
    }
    default: {
      break;
    }
    }
    return bytes;
  }

  size_t Writev(const struct iovec iov[], const int iov_len) {
    ssize_t bytes = writev(sock_, iov, iov_len);

    switch (bytes) {
    case -1: {
      common::HandleSyscallError("writev");
    }
    default: {
      break;
    }
    }
    return bytes;
  }

  size_t sendmsg_ZC(const msghdr * __msg){   //Do we need to overload the method?? 
    ssize_t bytes = sendmsg(sock_,__msg,MSG_ZEROCOPY);

    switch (bytes) {
      case -1: {
        common::HandleSyscallError("sendmsg");
      }
      default: {
        break;
      }
    }
    return bytes;
  }

  ssize_t sendmsg_ZC(struct msghdr* msg, Message* message) {
    if (is_closed_) {
        return -1;
    }
    completion_callback_ = [](Message* msg) { delete msg; };
    uint32_t seq = next_seq_++;
    pending_msgs_[seq] = message;
    return sendmsg(sock_, msg, MSG_ZEROCOPY);
  }

  Address GetMyAddress() {
    struct sockaddr_in name;
    socklen_t len = sizeof(name);
    if (getsockname(sock_, (struct sockaddr *)&name, &len) == -1) {
      common::HandleSyscallError("getsockname");
    }
    return Address(inet_ntoa(name.sin_addr), ntohs(name.sin_port));
  }
  static Address GetPeerAddress(int sock) {
    struct sockaddr_in name;
    socklen_t len = sizeof(name);
    if (getpeername(sock, (struct sockaddr *)&name, &len) == -1) {
      common::HandleSyscallError("getpeername");
    }
    return Address(inet_ntoa(name.sin_addr), ntohs(name.sin_port));
  }

  friend struct TCPConn;

private:
  std::function<void(Message*)> completion_callback_;
  std::unordered_map<uint32_t, Message*> pending_msgs_;
  std::thread completion_thread_;
  std::atomic<bool> should_stop{false};
  uint32_t next_seq_ = 0;
  bool is_closed_ = false;
  void cleanup() {
      // Call completion callback for all pending messages
      for (auto& [seq, msg] : pending_msgs_) {
          if (completion_callback_) {
              completion_callback_(msg);
          }
      }
      pending_msgs_.clear();
  }
  void processCompletions() {
      if (is_closed_) {
          return;
      }

      struct msghdr msg = {};
      char control[100];
      msg.msg_control = control;
      msg.msg_controllen = sizeof(control);

      while (recvmsg(sock_, &msg, MSG_ERRQUEUE | MSG_DONTWAIT) >= 0) {
          struct cmsghdr *cmsg = CMSG_FIRSTHDR(&msg);
          struct sock_extended_err *serr = (struct sock_extended_err *)CMSG_DATA(cmsg);
          
          if (serr->ee_errno == 0 && serr->ee_origin == SO_EE_ORIGIN_ZEROCOPY) {
              uint32_t seq = serr->ee_info;
              auto it = pending_msgs_.find(seq);
              if (it != pending_msgs_.end()) {
                  if (completion_callback_) {
                      completion_callback_(it->second);
                  }
                  pending_msgs_.erase(it);
              }
          }
      }
  }
  Conn(enum ConnProto proto, int sock)
      : proto_(proto), is_active_(true), sock_(sock) {}
};

struct TCPConn : Conn {
  const Address server_address_;
  const Address client_address_;

  TCPConn() = delete;

  TCPConn(const Address server_address)
      : Conn(ConnProto::TCP), server_address_(server_address) {}
  TCPConn(const Address server_address, const Address client_address)
      : Conn(ConnProto::TCP), server_address_(server_address),
        client_address_(client_address) {}

  TCPConn(const Address server_address, int sock)
      : Conn(ConnProto::TCP, sock), server_address_(server_address) {}
  TCPConn(const Address server_address, const Address client_address, int sock)
      : Conn(ConnProto::TCP,sock), server_address_(server_address), client_address_(client_address) {}
  ~TCPConn() { Close(); }
  inline int GetSocket() const { return sock_; }
  friend struct TCPServer;

private:
  TCPConn(int sock, const Address server_address, const Address client_address)
      : Conn(ConnProto::TCP, sock), server_address_(server_address),
        client_address_(client_address) {}
};

struct TCPServer : TCPConn {
  int addrlen = sizeof(server_ipv4_address_); // TODO: Fix it
  int backlog_;
  int sock_opt_ = 1;
  struct sockaddr_in server_ipv4_address_;

  TCPServer() = delete;
  TCPServer(const Address &server_address, const int backlog,
            std::string interface = "")
      : TCPConn(server_address), backlog_(backlog) {
    if (!interface.empty()) {
      common::BindToInterface(sock_, interface.c_str());
    }
    common::SetIPV4Addr(&server_ipv4_address_, server_address_.ip_.c_str(),
                        server_address_.port_);
    common::ReUseAddress(sock_, &sock_opt_);
    if (bind(sock_, (struct sockaddr *)&server_ipv4_address_,
             sizeof(server_ipv4_address_)) < 0) {
      common::HandleSyscallError("bind");
    }
  }
  ~TCPServer() { Close(); }
  bool StartListen() {
    if (listen(sock_, backlog_) < 0) {
      common::HandleSyscallError("listen");
    }
    return is_active_ = true;
  }
  TCPConn *GetNewConn() {
    int fd = GetNewConnFd();
    return new TCPConn(fd, server_address_, Conn::GetPeerAddress(fd));
  }

private:
  int GetNewConnFd() {
    // TODO: Move it to outside
    addrlen = sizeof(server_ipv4_address_);
    int new_socket = -1;
    if ((new_socket = accept(sock_, (struct sockaddr *)&server_ipv4_address_,
                             (socklen_t *)&addrlen)) < 0) {
      common::HandleSyscallError("accept");
    }
    return new_socket;
  }
};

struct TCPClient : TCPConn {
  int sock_opt_ = 1;
  void SetUpServerIpV4Address() {
    common::SetIPV4Addr(&server_ipv4_address_, server_address_.ip_.c_str(),
                        server_address_.port_);
    // TODO: Create constructor with specific client address
    // https://stackoverflow.com/questions/15673846/how-to-give-to-a-client-specific-ip-address-in-c
    /* no nee of it, as already set by server
    if (inet_pton(AF_INET, server_address_.ip_.c_str(),
                  &server_address_.ipv4_address_.sin_addr) <= 0) {
      common::HandleSyscallError("inet_pton");
    }
    */
  }
  void SetUpClientIpV4Address() {
    common::SetIPV4Addr(&client_ipv4_address_, client_address_.ip_.c_str(),
                        client_address_.port_);
    common::ReUseAddress(sock_, &sock_opt_);
    if (bind(sock_, (struct sockaddr *)&client_ipv4_address_,
             sizeof(client_ipv4_address_)) != 0) {
      common::HandleSyscallError("bind");
    }
  }

  TCPClient(const Address server_address, const std::string interface = "")
      : TCPConn(server_address) {
    if (!interface.empty()) {
      common::BindToInterface(sock_, interface.c_str());
    }
    SetUpServerIpV4Address();
  }

  TCPClient(const Address server_address, const Address client_address,
            const std::string interface = "")
      : TCPConn(server_address, client_address) {
    if (!interface.empty()) {
      common::BindToInterface(sock_, interface.c_str());
    }
    SetUpClientIpV4Address();
    SetUpServerIpV4Address();
  }

  ~TCPClient() { Close(); }

  bool Connect() {
    if (connect(sock_, (struct sockaddr *)&(server_ipv4_address_),
                sizeof(server_ipv4_address_)) == -1) {
      if (errno == 99) {
        return is_active_ = false;
      }
      common::HandleSyscallError("connect");
    }
    return is_active_ = true;
  }

  void Close() {
    CloseWrite();
    TCPConn::Close();
  }

private:
  struct sockaddr_in client_ipv4_address_;
  struct sockaddr_in server_ipv4_address_;
};
} // namespace net
} // namespace user
