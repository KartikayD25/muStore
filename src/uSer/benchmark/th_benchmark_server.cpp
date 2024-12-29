/*
 * Throughput Benchmark
 */
#include "epoll_client.hpp"
#include "epoll_server.hpp"
#include "helper.hpp"
#include "th_benchmark.hpp"

#include <unordered_map>

// Cahce size
#define HOLE_SIZE 49152

using namespace std;
using namespace user::net;

SerializationMethod serialization_method = SerializationMethod::COPY;

int library = 0; // 0: uSer 1: PB 2: FB
int msg_idx = -1, msg_type = -1;

struct Server_stats {
  double run_duration_sec_ = 0;
  size_t open_conn_ = 0;
  size_t closed_conn_ = 0;
  size_t bytes_read_ = 0;
  size_t bytes_written_ = 0;

  void PrintStats() {
    double bits_read_ = bytes_read_ * 8;
    double bits_written_ = bytes_written_ * 8;
    double divider = 1000000;
    auto method_string =
        (serialization_method == SerializationMethod::COPY ? "COPY" : "SG");
    std::cout << "----------"
              << (serialization_method == SerializationMethod::COPY ? "COPY"
                                                                    : "SG")
              << "----------" << std::endl;
    std::cout << "OpenConn " << open_conn_ << " ClosedConn " << closed_conn_
              << " bytes_read " << bytes_read_ << " bytes_written "
              << bytes_written_ << " bytes_total "
              << (bytes_read_ + bytes_written_) << " duration(s) "
              << run_duration_sec_ << std::endl;
    std::cout << "TH(Mbps) " << method_string << "\t"
              << " RPS "
              << (closed_conn_ / static_cast<size_t>(run_duration_sec_))
              << " read " << (bits_read_ / (divider * run_duration_sec_))
              << " write " << (bits_written_ / (divider * run_duration_sec_))
              << " total "
              << ((bits_read_ + bits_written_) / (divider * run_duration_sec_))
              << std::endl;
  }
} stats;

size_t req_size = net::common::BUFFER_SIZE;
char *client_req_buf = nullptr;

user::DeserialLatency *deser_lat_msgr_ = nullptr;

// msg_t **msgs = nullptr;
int N_MSG = 0;

typedef struct ServerMsg {
  bool sent_{false};
} ServerMsg;
unordered_map<int, ServerMsg> server_sock_msg_map_;

// char *svr_client_req_buf = Helper::Allocator::AllocZeroed<char>(req_size);

NetworkEvent onServerNetEvent(NetworkEvent net_ev) {
  switch (net_ev.net_event_) {
  case NetEvent::NEW: {
    server_sock_msg_map_[net_ev.conn_->GetSocket()] = ServerMsg();
    stats.open_conn_++;
    // std::cout << "NEW_CONN open: " << stats.open_conn_ << std::endl;
    break;
  }
  case NetEvent::WRITE: {
    std::cout << "WRITE\n";
    NOT_IMPLEMENTED;
    break;
  }
  case NetEvent::READ: {
    // std::cout << "READDDING "
    //        << server_sock_msg_map_[net_ev.conn_->GetSocket()].sent_
    //      << std::endl;
    // Read
    // if (!server_sock_msg_map_[net_ev.conn_->GetSocket()].sent_) {
    // server_sock_msg_map_[net_ev.conn_->GetSocket()].sent_ = true;
    // std::cout << "ReadingRequest...\n";
    char *buf = (char *)calloc(req_size, 1);
    size_t bytes = net_ev.conn_->Read(buf, req_size);
    stats.bytes_read_ += bytes;
    std::cout << "Read bytes: " << bytes << std::endl;
    Msg_M2* msg = nullptr;
    time_unit_t des_time = 0;
    if (library == 0) {
        {
            Helper::Timer<Helper::nanosecond_t> _(des_time);
            msg = Msg_M2::CreateMsg(buf, req_size);
        }
        deser_lat_msgr_->AddRecord(static_cast<double>(des_time));
      bytes = msg->SerialiseAndSend(net_ev.conn_, serialization_method);
      stats.bytes_written_ += bytes;
      std::cout << "des_time " << des_time<< std::endl;
    exit(0);
      delete msg;
    }
#ifdef PROTOBUF
    if (library == 1) {
        {
            Helper::Timer<Helper::nanosecond_t> _(des_time);
      msg = Msg_M2::CreateMsgFromPB(buf, req_size);
        }
        deser_lat_msgr_->AddRecord(static_cast<double>(des_time));
      bytes = msg->SerialiseAndSendPB(net_ev.conn_);
      stats.bytes_written_ += bytes;
      std::cout << "des_time " << des_time << std::endl;
    exit(0);
      delete msg;
    }
#endif
#ifdef FLATBUF
    if (library == 2) {
        {
            Helper::Timer<Helper::nanosecond_t> _(des_time);
      msg = Msg_M2::CreateMsgFromFB(buf, req_size);
        }
        deser_lat_msgr_->AddRecord(static_cast<double>(des_time));
      bytes = msg->SerialiseAndSendFB(net_ev.conn_);
      stats.bytes_written_ += bytes;
      std::cout << "des_time " << des_time << std::endl;
    exit(0);
      delete msg;
    }
#endif
    std::cout << "Wrote bytes: " << bytes << "library: " << library << std::endl;
    free(buf);
    buf = nullptr;
    // TODO: ASSERT BYTES
    //}
    break;
  }
  case NetEvent::CLOSE: {
    stats.closed_conn_++;
    assert(false);
    break;
  }
  default: { NOT_IMPLEMENTED; }
  }
  net_ev.net_event_ = NetEvent::INVALID;
  return net_ev;
}

void Server(int n_msg, int type, int size, std::string &device) {
  std::cout << CURR_READABLE_TIME << " Start Allocating\n";

  std::cout << CURR_READABLE_TIME << " End Allocating\n";
  Address server_address("0.0.0.0", port);
  EpollServer es(server_address, device);
  std::cout << CURR_READABLE_TIME << " Starting Server\n";
  es.Serve(onServerNetEvent);
  std::cout << CURR_READABLE_TIME << " End Server\n";
  stats.PrintStats();
  user::LatencyRecorderMgr::Get()->DumpAggregatedStat();
  deser_lat_msgr_->Print();
}

int main(int argc, char *argv[]) {
  Helper::BindThisThread(2);
#ifdef RUN_FIXED_DURATION
  stats.run_duration_sec_ = std::stol(Helper::GetEnv("RUN_DURATION"));
  assert(stats.run_duration_sec_ > 0);
  std::cout << "RUN_DURATION " << stats.run_duration_sec_ << std::endl;
#endif
  int option;
  std::string server_ip = "";
  int serialization_tech = -1;
  std::string device = "";
  while ((option = getopt(argc, argv, "t:m:c:d:t:l:hs:")) != -1) {
    switch (option) {
    case 'm':
      msg_idx = atoi(optarg);
      break;
    case 't':
      msg_type = atoi(optarg);
      break;
    case 'c':
      server_ip = optarg;
      break;
    case 'd':
      device = optarg;
      break;
    case 's':
      serialization_tech = atoi(optarg);
      break;
    case 'l':
      library = atoi(optarg);
      break;
    case 'h':
    default:
      printf("Help/Usage Example\n");
      abort();
    }
  }

  assert(msg_idx >= 0 && msg_type >= 0);
  if (library == 0) {
  initUser();
    user::net::common::BUFFER_SIZE = USER_BUFFER_SIZES[msg_idx][msg_type];
    deser_lat_msgr_ = new user::DeserialLatency("deserial", (int)MESSAGE_SIZES[msg_idx], msg_type);
  } else if(library == 1) {
    user::net::common::BUFFER_SIZE = PB_BUFFER_SIZES[msg_idx][msg_type];
    deser_lat_msgr_ = new user::DeserialLatency("deserial", (int)MESSAGE_SIZES[msg_idx], msg_type);
  } else if(library == 2){
    user::net::common::BUFFER_SIZE = FB_BUFFER_SIZES[msg_idx][msg_type];
    deser_lat_msgr_ = new user::DeserialLatency("deserial", (int)MESSAGE_SIZES[msg_idx], msg_type);
  } else {
    NOT_IMPLEMENTED;
  }

  if (serialization_tech == 0) {
    serialization_method = SerializationMethod::COPY;
  } else if (serialization_tech == 1) {
    serialization_method = SerializationMethod::SG;
  } else if(library == 0) {
    std::cerr << "Invalid Serialization technique: " << serialization_tech
              << " 0:COPY 1:SG" << std::endl;
    return -1;
  }

  // TODO: Change it to 10
  int n_message = 1,
      size = net::common::BUFFER_SIZE; // MESSAGE_SIZES[msg_idx];
  req_size = net::common::BUFFER_SIZE;
  user::LatencyRecorderMgr::Init()->StartNewStats(
      library * 10 + msg_type, serialization_method, MESSAGE_SIZES[msg_idx], 2);
  std::cout << "msg_idx: " << msg_idx << " size: " << size
            << std::endl;
  Server(n_message, msg_type, size, device);
  std::cout << CURR_READABLE_TIME << " Closing\n";
  return 0;
}
