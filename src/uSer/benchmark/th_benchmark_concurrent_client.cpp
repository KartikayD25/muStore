/*
 * Throughput Benchmark
 */
#include "epoll_client.hpp"
#include "epoll_server.hpp"
#include "helper.hpp"
#include "th_benchmark.hpp"

#include <atomic>
#include <thread>
#include <unordered_map>

using namespace std;
using namespace user::net;
using Helper::time_unit_t;

size_t max_clients = 1;

int library = -1; // 0: uSer 1: PB 2: FB
int msg_idx = -1, msg_type = -1;

size_t req_size_ = 0; //user::net::common::BUFFER_SIZE; // 2048;
size_t resp_size_ = 0; //user::net::common::BUFFER_SIZE;

std::mutex mtx; // Common mutex
typedef struct thread_param_t {
  Address server_address_;
  std::string device_;
  int cpu_id_{-1};
  std::atomic<uint32_t> &started_thread_cnt_;
  uint32_t n_threads_ = 1;
  thread_param_t(Address server_address, std::string device, int cpu_id,
                 std::atomic<uint32_t> &thread_cnt, uint32_t n_threads)
      : server_address_(server_address), device_(device), cpu_id_(cpu_id),
        started_thread_cnt_(thread_cnt), n_threads_(n_threads) {}
  // thread_param_t(Address server_address, int cpu_id)
  //    : server_address_(server_address), cpu_id_(cpu_id) {}
} thread_param_t;

struct MessageInfo {
  char *req_buf_ = nullptr;
  char *resp_buf_ = nullptr;
  MessageInfo() {
    //req_buf_ = Helper::Allocator::AllocZeroed<char>(req_size_);
    //assert(req_buf_ != nullptr);
    resp_buf_ = Helper::Allocator::AllocZeroed<char>(resp_size_);
    assert(resp_buf_ != nullptr);
    //Helper::Random::FillRandomReadableASCII(req_buf_, req_size_);
  }
};

class EchoClient {
public:
  size_t bytes_read_ = 0, bytes_written_ = 0;
  time_unit_t start_time_ = 0, end_time_ = 0;
  bool req_sent_{false};
  MessageInfo *msg_info{nullptr};
  Msg_M2 *msg_{nullptr};
  EchoClient() {
      msg_info = new MessageInfo();
      msg_ = new Msg_M2(msg_idx, msg_type);
  }

  void PrintStats() {
    std::cout << "bytes_read " << bytes_read_ << " bytes_written "
              << bytes_written_ << " bytes_total "
              << (bytes_read_ + bytes_written_) << " duration(s) "
              << (end_time_ - start_time_) << std::endl;
    double duration = (end_time_ - start_time_);
#ifdef RUN_FIXED_DURATION
    duration = std::stoi(Helper::GetEnv("RUN_DURATION"));
    assert(duration > 0);
#endif
    std::cout << "TH (BPS) "
              << "Read " << (bytes_read_ / duration) << " Write "
              << (bytes_written_ / duration) << " Total "
              << ((bytes_written_ + bytes_read_) / duration) << std::endl;
  }

  void sendData(NetworkEvent net_ev) {
      ssize_t req_size = 0;
      if(library == 0) {
          req_size = msg_->SerialiseAndSend(net_ev.conn_, SerializationMethod::COPY);
  } 
#ifdef PROTOBUF
      else if(library == 1) { // PB
          req_size = msg_->SerialiseAndSendPB(net_ev.conn_);
      }
#endif
#ifdef FLATBUF
      else if(library == 2) { // FB
          req_size = msg_->SerialiseAndSendFB(net_ev.conn_);
      }
#endif
        bytes_written_ += req_size;
  }
  NetworkEvent onClientEvent(NetworkEvent net_ev) {
    // auto msg_info = thread_id_reqinfo_map[std::this_thread::get_id()];
    switch (net_ev.net_event_) {
    case NetEvent::NEW: {
      // Not possible for client
      // socket_req_info_map[net_ev.conn_->GetSocket()] = ReqInfo();
      start_time_ = Helper::Timer<Helper::second_t>::CurrentTime();
      break;
    }
    case NetEvent::READ: {
      //std::cout << "READ " << net_ev.conn_->GetSocket() << std::endl;
      //assert(net_ev.conn_->Read(msg_info->resp_buf_, resp_size_) == resp_size_);
      auto read_size = net_ev.conn_->Read(msg_info->resp_buf_, resp_size_);
      assert(read_size > 0);
      std::cout << "ReadSize: " << read_size << " EpectedSize: " << resp_size_ << std::endl;
      assert(read_size == resp_size_);
      bytes_read_ += resp_size_;
      // std::cout << "WRITE " << net_ev.conn_->GetSocket() << std::endl;;
      //assert(net_ev.conn_->Write(msg_info->req_buf_, req_size_) == req_size_);
      //bytes_written_ += req_size_;
      //  auto req_size = msg_->SerialiseAndSend(net_ev.conn_, SerializationMethod::COPY);
      //  bytes_written_ += req_size;
          sendData(net_ev);
      break;
      // return net_ev;
    }
    case NetEvent::WRITE: {
      if (req_sent_ == false) {
        // std::cout << "WRITE(First) " << net_ev.conn_->GetSocket() <<
        // std::endl;
        //assert(net_ev.conn_->Write(msg_info->req_buf_, req_size_) == req_size_);
        //auto req_size = msg_->SerialiseAndSend(net_ev.conn_, SerializationMethod::COPY);
        // bytes_written_ += req_size;
          sendData(net_ev);
        req_sent_ = true;
      }
      break;
    }
    case NetEvent::CLOSE: {
      end_time_ = Helper::Timer<Helper::second_t>::CurrentTime();
      break;
    }
    default: { NOT_IMPLEMENTED; }
    }
    net_ev.net_event_ = NetEvent::INVALID;
    return net_ev;
  }
};

void Client(thread_param_t param) {
  Helper::BindThisThread(param.cpu_id_);
  Address server_address = param.server_address_;
  std::cout << "starting... " << param.cpu_id_ << std::endl;
  EpollClient epc(server_address, max_clients, param.device_);
  std::cout << "started " << param.cpu_id_ << std::endl;
  epc.BindAt(param.cpu_id_ + 1);
  EchoClient client;
  auto callback_func =
      std::bind(&EchoClient::onClientEvent, &client, std::placeholders::_1);
  param.started_thread_cnt_++;
  while (param.started_thread_cnt_ != param.n_threads_)
    ;
  std::cout << "Client started: " << param.cpu_id_ << std::endl;
  epc.StartClient(callback_func);
  // epc.StartClient(onClientEvent);
  {
    std::lock_guard<std::mutex> _(mtx);
    client.PrintStats();
  }
}

int main(int argc, char *argv[]) {
  int option;
  uint32_t n_threads = 1;
  std::string server_ip = "";
  std::string device = "";
  while ((option = getopt(argc, argv, "c:d:m:t:p:l:h")) != -1) {
    switch (option) {
    case 'c':
      server_ip = optarg;
      break;
    case 'd':
      device = optarg;
      break;
    case 'p':
      n_threads = std::stoi(optarg);
      break;
    case 't':
      msg_type = atoi(optarg);
      break;
    case 'm':
      msg_idx = std::stoi(optarg);
      break;
    case 'l':
      library = std::stoi(optarg);
      break;
    case 'h':
    default:
      printf("Help/Usage Example\n");
      abort();
    }
  }
  assert(n_threads < std::thread::hardware_concurrency());
  std::thread *threads[n_threads];


  assert(msg_idx >= 0 && msg_type >= 0);
  if(library == 0) {
      user::net::common::BUFFER_SIZE = USER_BUFFER_SIZES[msg_idx][msg_type];
  } else if(library == 1) {
      user::net::common::BUFFER_SIZE = PB_BUFFER_SIZES[msg_idx][msg_type];
  } else {
      user::net::common::BUFFER_SIZE = FB_BUFFER_SIZES[msg_idx][msg_type];
      std::cout << "FB: SIZE: " << user::net::common::BUFFER_SIZE << std::endl;
  }
  //resp_size_ = MESSAGE_SIZES[msg_idx];
  req_size_ = user::net::common::BUFFER_SIZE;
  resp_size_ = user::net::common::BUFFER_SIZE;
  Address server_address(server_ip, port);

  initUser();
    user::LatencyRecorderMgr::Init()->StartNewStats(0, SerializationMethod::COPY, net::common::BUFFER_SIZE, 2);
  std::atomic<uint32_t> started_thread_cnt(0);
  uint32_t cpu_id = 3;
  std::cout << "n_threads: " << n_threads << std::endl;
  for (uint32_t i = 0; i < n_threads; i++) {
    threads[i] = new std::thread(
        Client, thread_param_t(server_address, device, cpu_id + i * 2,
                               started_thread_cnt, n_threads));
  }
  for (uint32_t i = 0; i < n_threads; i++) {
    if (threads[i]->joinable()) {
      threads[i]->join();
    }
  }
  std::cout << "FINISHED\n";
  return 0;
}
