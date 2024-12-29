#ifndef EPOLL_INSTANCE_HPP
#define EPOLL_INSTANCE_HPP

#include "common.hpp"
#include "epoll_mgr.hpp"

#include <cassert>
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <vector>

using namespace user::net;

struct EpollInstance {
  int ep_fd_;
  int default_events = EPOLLOUT | EPOLLIN | EPOLLRDHUP | EPOLLERR | EPOLLET;
  const uint32_t max_events_;
  bool is_active_{false};

  int timeout_ = -1; // 0: Return immediately
  struct epoll_event *ev_list_ = nullptr;
  size_t event_process_idx_ = 0;  // Event to be processed
  size_t num_pending_events_ = 0; // Total ready fds

  void flush() {
    std::cerr << "NOT_IMPLEMENTED" << std::endl;
    abort();
  }
  EpollInstance(uint32_t max_events, int timeout = -1)
      : max_events_(max_events), is_active_(true), timeout_(timeout),
        ev_list_(dynamic_cast<struct epoll_event *>(
            new struct epoll_event[max_events_])) {
    assert(ev_list_ != nullptr);
    ep_fd_ = epoll_create1(0);
    if (ep_fd_ == -1) {
      common::HandleSyscallError("epoll_create1");
    }
  }

  void AddConnection(int sock, int special_events = 0,
                     bool special_events_only = false) {
    struct epoll_event *ev = new struct epoll_event();
    ev->events = special_events_only ? special_events
                                     : (default_events | special_events);
    ev->data.fd = sock;
    int r = -1;
    if ((r = epoll_ctl(ep_fd_, EPOLL_CTL_ADD, sock, ev)) != 0) {
      common::HandleSyscallError("epoll_ctl_ADD");
    }
  }

  void RemoveConnection(int sock) {
    int r = -1;
    if ((r = epoll_ctl(ep_fd_, EPOLL_CTL_DEL, sock, NULL)) != 0) {
      common::HandleSyscallError("epoll_ctl_DEL");
    }
  }

  EpollEventInfo CollectEvents() {
    if (event_process_idx_ == num_pending_events_) {
      event_process_idx_ = num_pending_events_ = 0; // reset
      // std::cout << "Waiting " << timeout_ << std::endl ;
      int num_ready_fd = epoll_wait(ep_fd_, ev_list_, max_events_, timeout_);
      if (num_ready_fd == -1) {
        if (errno == EINTR) {
          return EpollEventInfo{.event_ = EpollEvents::INVALID, .sock_ = -1};
        }
        common::HandleSyscallError("epoll_wait");
      }
      num_pending_events_ = static_cast<size_t>(num_ready_fd);
      // std::cout << "num_events: " << num_pending_events_ << std::endl;
      if (num_pending_events_ == 0) {
        if (timeout_ != -1) {
          return EpollEventInfo{.event_ = EpollEvents::TIMEOUT, .sock_ = -1};
        }
        return EpollEventInfo{.event_ = EpollEvents::INVALID, .sock_ = -1};
      }
    }

    // Process pending events: Events that are received by the epoll but not yet
    // processes
    return EventToEpollEventInfo(ev_list_ + (event_process_idx_++));
  }

  EpollEventInfo EventToEpollEventInfo(struct epoll_event *ev) {
    /*
std::cout << std::endl << "[";
for (auto event :
   std::vector<std::pair<int, std::string>>{{EPOLLOUT, "EPOLLOUT"},
                                            {EPOLLIN, "EPOLLIN"},
                                            {EPOLLRDHUP, "EPOLLRDHUP"},
                                            {EPOLLERR, "EPOLLERR"}}) {
if (ev->events & event.first) {
  std::cout << event.second << ":" << ev->data.fd << " ";
}
}
std::cout << "]" << std::endl;
*/
    // Sequence must be the below
    if (ev->events & EPOLLRDHUP) {
      // std::cout << "DISCONNECT\n";
      return EpollEventInfo{.event_ = EpollEvents::CLOSE_CONN,
                            .sock_ = ev->data.fd};
    }
    if (ev->events & EPOLLIN & EPOLLOUT) {
      //  std::cout << "BOTH\n";
      assert(false);
      return EpollEventInfo{.event_ = EpollEvents::READ_WRITE,
                            .sock_ = ev->data.fd};
    }
    if (ev->events & EPOLLIN) {
      //    std::cout << "EPOLLIN\n";
      return EpollEventInfo{.event_ = EpollEvents::READ, .sock_ = ev->data.fd};
    }
    if (ev->events & EPOLLOUT) {
      //      std::cout << "EPOLLOUT\n";
      return EpollEventInfo{.event_ = EpollEvents::WRITE, .sock_ = ev->data.fd};
    }
    if (ev->events & EPOLLERR) {
      NOT_IMPLEMENTED;
    }
    return EpollEventInfo{.event_ = EpollEvents::INVALID, .sock_ = ev->data.fd};
  }
};

#endif
