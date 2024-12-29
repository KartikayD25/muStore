#ifndef EPOLL_MGR_HPP
#define EPOLL_MGR_HPP

#include <functional>
#include <unordered_map>

//#include "epoll_instance.hpp"
#include "helper.hpp"
#include "net.hpp"

#define MAX_EVENTS 1000
#define TCP_BACKLOG 4096

using namespace user::net;

enum class NetEvent { INVALID, NEW, READ, WRITE, READ_WRITE, CLOSE };

struct NetworkEvent {
  NetEvent net_event_;
  TCPConn *conn_;
  NetworkEvent(NetEvent net_event, TCPConn *conn)
      : net_event_(net_event), conn_(conn) {}
};

enum class EpollEvents {
  INVALID,
  NEW_CONN,
  READ_WRITE,
  READ,
  WRITE,
  CLOSE_CONN,
  TIMEOUT
};

static std::string EpollEventsToStr(EpollEvents ev) {
  switch (ev) {
  case EpollEvents::INVALID:
    return "INVALID";
  case EpollEvents::NEW_CONN:
    return "NEW_CON";
  case EpollEvents::READ_WRITE:
    return "READ_WRITE";
  case EpollEvents::READ:
    return "READ";
  case EpollEvents::WRITE:
    return "WRITE";
  case EpollEvents::CLOSE_CONN:
    return "CLOSE_CONN";
  case EpollEvents::TIMEOUT:
    return "TIMEOUT";
  default:
    NOT_IMPLEMENTED;
  }
}

struct EpollEventInfo {
  EpollEvents event_;
  int sock_;
  EpollEventInfo(EpollEvents event, int sock) : event_(event), sock_(sock) {}
};

#endif
