#ifndef EPOLL_SERVER_HPP
#define EPOLL_SERVER_HPP

#include "epoll_instance.hpp"
#include "epoll_mgr.hpp"
#include "global_timer.hpp"

using namespace user::net;

struct EpollServer {
  const uint32_t max_events_{MAX_EVENTS};
  bool is_active_{false};
  TCPServer *tcp_server_{nullptr};
  EpollInstance *ep_instance_{nullptr};

  std::unordered_map<int, TCPConn *> sock_to_conn_map_;

  int timeout_ = 1000; // ms
#ifdef RUN_FIXED_DURATION
  long run_duration_sec_ = 0;
#endif

  EpollServer(common::Address address, std::string interface = "")
      : is_active_(true),
        tcp_server_(new TCPServer(address, TCP_BACKLOG, interface)),
        ep_instance_(new EpollInstance(max_events_, timeout_)) {
    ep_instance_->AddConnection(tcp_server_->GetSocket());
    assert(tcp_server_->StartListen());

#ifdef RUN_FIXED_DURATION
    run_duration_sec_ = std::stoi(Helper::GetEnv("RUN_DURATION"));
    assert(run_duration_sec_ > 0);
#endif
  }

  ~EpollServer() {
    // std::cout << "Closed SERVER----------------------\n\n";
    is_active_ = false;
    delete ep_instance_;
    delete tcp_server_;
  }

  void Serve(std::function<NetworkEvent(NetworkEvent)> callback_func_) {
    while (is_active_) {
#ifdef RUN_FIXED_DURATION
      // auto start_time = Helper::Timer<Helper::second_t>::CurrentTime();
      /*
    auto curr_time = Helper::Timer<Helper::second_t>::CurrentTime();
    if ((curr_time - start_time) >= run_duration_sec_) {
      is_active_ = false;
      break;
    }
    */
      auto instance = GlobalTimerMgr<Helper::microsecond_t>::GetInstance();
      if (instance != nullptr && instance->IsOver()) {
        is_active_ = false;
        break;
      }
#endif
      EpollEventInfo ev_info = ep_instance_->CollectEvents();
      switch (ev_info.event_) {
      case EpollEvents::INVALID: {
        break;
      }
      case EpollEvents::TIMEOUT: {
        break;
      }
      case EpollEvents::READ_WRITE: {
        std::cout << "BOTH\n";
        assert(false);
      }
      case EpollEvents::READ: {
        if (ev_info.sock_ == tcp_server_->GetSocket()) {
          TCPConn *new_conn = tcp_server_->GetNewConn();
          // ep_instance_->AddConnection(new_conn->GetSocket());
          ep_instance_->AddConnection(new_conn->GetSocket(),
                                      EPOLLIN | EPOLLRDHUP | EPOLLERR | EPOLLET,
                                      true);
          // std::cout << "NewConn: " << new_conn->GetSocket() << " "
          //        << new_conn->client_address_ << std::endl;
          sock_to_conn_map_[new_conn->GetSocket()] = new_conn;
#ifdef RUN_FIXED_DURATION
          GlobalTimerMgr<Helper::microsecond_t>::StartTimer(run_duration_sec_ *
                                                            1e6);
#endif
          ProcessAction(callback_func_(NetworkEvent(NetEvent::NEW, new_conn)));
        } else {
          TCPConn *conn = sock_to_conn_map_[ev_info.sock_];
          // std::cout << "Read: " << ev_info.sock_ << " " <<
          // conn->client_address_
          //        << std::endl;
          ProcessAction(callback_func_(NetworkEvent(NetEvent::READ, conn)));
        }
        break;
      }
      case EpollEvents::WRITE: {
        TCPConn *conn = sock_to_conn_map_[ev_info.sock_];
        // std::cout << "WRITE: " << ev_info.sock_ << " " <<
        // conn->client_address_
        //        << std::endl;
        ProcessAction(callback_func_(NetworkEvent(NetEvent::WRITE, conn)));
        break;
      }
      case EpollEvents::CLOSE_CONN: {
        TCPConn *conn = sock_to_conn_map_[ev_info.sock_];
        // std::cout << "Close: " << ev_info.sock_ << " " <<
        // conn->client_address_
        //        << std::endl;
        callback_func_(NetworkEvent(NetEvent::CLOSE,
                                    conn)); // No need to process return value
        ep_instance_->RemoveConnection(conn->GetSocket());
        conn->Close();
        sock_to_conn_map_.erase(ev_info.sock_);
        delete conn;
        break;
      }
      default:
        std::cerr << "EventNotHandled. Event: "
                  << EpollEventsToStr(ev_info.event_) << std::endl;
        NOT_IMPLEMENTED;
        break;
      }
    }
  }

  void ProcessAction(NetworkEvent net_ev) {
    switch (net_ev.net_event_) {
    case NetEvent::CLOSE: {
      // std::cout << "SERVERACTION:: "
      //        << "CLOSE " << net_ev.conn_->GetSocket() << std::endl;
      ep_instance_->RemoveConnection(net_ev.conn_->GetSocket());
      net_ev.conn_->Close();
      delete net_ev.conn_;
      break;
    }
    default: { break; }
    }
  }
};
#endif
