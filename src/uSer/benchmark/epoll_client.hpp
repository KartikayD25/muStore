#ifndef EPOLL_CLIENT_HPP
#define EPOLL_CLIENT_HPP

#include "epoll_instance.hpp"
#include "epoll_mgr.hpp"
#include "helper.hpp"

#include <mutex>
#include <thread>

using namespace user::net;

struct EpollClient {
  const uint32_t max_clients_{MAX_EVENTS};
  const int timeout_ = 1000; // ms
  bool is_active_{false};
  Address server_address_;
  Address client_address_;
  std::string interface_;
  bool reuse_addr_{false};
  EpollInstance *ep_instance_{nullptr};

  std::unordered_map<int, TCPClient *> sock_to_client_map_;

  size_t stats_total_conn_ = 0;
  size_t stats_open_conn_ = 0;
  size_t stats_closed_conn_ = 0;

#ifdef RUN_FIXED_DURATION
  long run_duration_sec_ = 0;
#endif

  int extra_cpu_ = -1;

  std::mutex mtx_;
  EpollClient(common::Address address, int max_clients,
              std::string interface = "")
      : max_clients_(max_clients), is_active_(true), server_address_(address),
        ep_instance_(new EpollInstance(max_clients_ * MAX_EVENTS, timeout_)) {
#ifdef RUN_FIXED_DURATION
    run_duration_sec_ = std::stoi(Helper::GetEnv("RUN_DURATION"));
    assert(run_duration_sec_ > 0);
#endif
  }
  EpollClient(common::Address address, common::Address client_address,
              int max_clients, std::string interface = "")
      : max_clients_(max_clients), is_active_(true), server_address_(address),
        client_address_(client_address), reuse_addr_(true),
        ep_instance_(new EpollInstance(max_clients_, timeout_)) {
#ifdef RUN_FIXED_DURATION
    run_duration_sec_ = std::stoi(Helper::GetEnv("RUN_DURATION"));
    assert(run_duration_sec_ > 0);
#endif
  }

  ~EpollClient() {
    is_active_ = false;
    delete ep_instance_;
  }
  void BindAt(int cpu_id) { extra_cpu_ = cpu_id; }

  /*
   * Connect 16 clients and send data over it
   */
  void StartClient(std::function<NetworkEvent(NetworkEvent)> callback_func) {
    // Connect
    // std::thread th([&]() {
    // Helper::BindThisThread(extra_cpu_);
    // std::cout << "MAX_CLIENTS: " << max_clients_ << std::endl;
    for (size_t i = 0; i < max_clients_; i++) {
      // while (true) {
      TCPClient *client = nullptr; // new TCPClient(server_address_,
                                   // interface_);
      if (reuse_addr_) {
        client = new TCPClient(server_address_, client_address_, interface_);
      } else {
        client = new TCPClient(server_address_, interface_);
      }
      // assert(client->Connect());
      if (!client->Connect()) {
        delete client;
        continue;
      }
      mtx_.lock();
      sock_to_client_map_[client->GetSocket()] = client;
      mtx_.unlock();
      ep_instance_->AddConnection(client->GetSocket());
      stats_total_conn_++;
    }
    //});
    while (is_active_) {
#ifdef RUN_FIXED_DURATION
      // auto start_time = Helper::Timer<Helper::second_t>::CurrentTime();
      // std::cout << "DURATION: " << run_duration_sec_ << std::endl;
      /*
    auto curr_time = Helper::Timer<Helper::second_t>::CurrentTime();
    if ((curr_time - start_time) >= run_duration_sec_) {
      is_active_ = false;
      break;
    }*/
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
        assert(false);
      }
      case EpollEvents::READ: {
        mtx_.lock();
        TCPClient *client = sock_to_client_map_[ev_info.sock_];
        mtx_.unlock();
        (callback_func(NetworkEvent(NetEvent::READ, client)));
        // ProcessAction(callback_func(NetworkEvent(NetEvent::READ, client)));
        break;
      }
      case EpollEvents::WRITE: {
        mtx_.lock();
        TCPClient *client = sock_to_client_map_[ev_info.sock_];
        mtx_.unlock();
        (callback_func(NetworkEvent(NetEvent::WRITE, client)));
        // ProcessAction(callback_func(NetworkEvent(NetEvent::WRITE,
        // client)));
        break;
      }
      case EpollEvents::CLOSE_CONN: {
        mtx_.lock();
        // std::cout << "CLOSE " << ev_info.sock_ << std::endl;
        TCPClient *client = sock_to_client_map_[ev_info.sock_];
        mtx_.unlock();
        callback_func(NetworkEvent(NetEvent::CLOSE, client));

        client->Close();
        mtx_.lock();
        sock_to_client_map_.erase(ev_info.sock_);
        mtx_.unlock();
        delete client;
        stats_closed_conn_++;
        is_active_ = false;
        break;
      }
      default: {
        std::cerr << "EventNotHandled. Event: "
                  << EpollEventsToStr(ev_info.event_) << std::endl;
        NOT_IMPLEMENTED;
        break;
      }
      }
      usleep(10);
    }
    // th.join();
    if (false) {
      std::cout << " MAX_CONN: " << max_clients_
                << " TOTAL_OPEN_CONN: " << stats_total_conn_
                << " TOTAL_CLOSED_CONN: " << stats_closed_conn_
                << " MAPSIZE: " << sock_to_client_map_.size()
                << " TH(rps): " << (stats_closed_conn_ / run_duration_sec_)
                << std::endl;
    }
  }

  void ProcessAction(NetworkEvent net_ev) {
    switch (net_ev.net_event_) {
    case NetEvent::CLOSE: {
      // std::cout << "SERVERACTION:: "
      //        << "CLOSE " << net_ev.conn_->GetSocket() << std::endl;
      mtx_.lock();
      sock_to_client_map_.erase(net_ev.conn_->GetSocket());
      mtx_.unlock();
      ep_instance_->RemoveConnection(net_ev.conn_->GetSocket());
      net_ev.conn_->Close();
      delete net_ev.conn_;
      stats_closed_conn_++;
      break;
    }
    default: { break; }
    }
  }
};
#endif
