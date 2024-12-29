/*
 * Throughput Benchmark
 */
#ifndef TH_BENCHMARK_HPP
#define TH_BENCHMARK_HPP

#include <iostream>

#include "../src/latency_tool.hpp"
#include "../src/user.hpp"
#include "epoll_server.hpp"
#include "helper.hpp"

#ifdef PROTOBUF
#include "synthetic.pb.h"
#endif

#ifdef FLATBUF
#include "synthetic_generated.h"
#endif

#define KB *1024

using namespace std;
// using namespace common;
using namespace user;
using namespace user::net;
using namespace user::net::common;

port_t port = 8089;

// in Bytes
// static size_t MESSAGE_SIZES[] = {32,     64,     128,    512,     1 KB,
//                                 2 KB,   5 KB,   10 KB,  20 KB,   50 KB,
//                                 100 KB, 256 KB, 512 KB, 1000 KB, 10000 KB};
// 512 -> 544 type: 1
static size_t MESSAGE_SIZES[] = {1 KB, 10 KB, 30 KB, 100 KB, 256 KB};
// static size_t USER_BUFFER_SIZES[] = {1084, 1084, 1084};
static std::vector<std::vector<int>> USER_BUFFER_SIZES{{1040, 1084},
                                                       {10256, 10650},
                                                       {30736, 30788},
                                                       {102416, 102468},
                                                       {262160, 262220}};
// 50KB (2)-> COPY: 23541.141   SG: 21741.467
// 20 KB  -> 3micro,
// 256 KB -> 16micro, 27micro
// 20KB: {20496, 20548}

static std::vector<std::vector<int>> PB_BUFFER_SIZES{{1027, 1048},
                                                     {10243, 10616},
                                                     {30724, 30754},
                                                     {102404, 102436},
                                                     {262148, 262190}};
static std::vector<std::vector<int>> FB_BUFFER_SIZES{{1027, 1152},
                                                     {10243, 10736},
                                                     {30724, 30888},
                                                     {102404, 102560},
                                                     {262148, 262328}};

const size_t MESSAGE_SIZES_LEN =
    sizeof(MESSAGE_SIZES) / sizeof(MESSAGE_SIZES[0]);

std::string CreateRandomString(size_t len) {
  static char c = 'a';

  std::string str(len, c /*'a'*/);
  c++;
  // Helper::Random::FillRandomReadableASCII(str, len);
  return str;
}
/*
void FillString(std::vector<std::string> &str_vec, size_t total_len, int type) {
  if (type == 0) { // size 1024 ==> 2116
      //vector<int> sizes1{60, 212, 7, 328, 411, 6}; // 1024
      vector<int> sizes1{5*1024}; // 1024
      auto n = sizes1.size();
    assert(n < 32);
    for (size_t i = 0; i < n; i++) {
      str_vec.push_back(CreateRandomString(sizes1[i]));
    }
  }
}
*/
void FillString(std::vector<std::string> &str_vec, std::vector<int> &sizes) {
  for (size_t i = 0; i < sizes.size(); i++) {
    str_vec.push_back(CreateRandomString(sizes[i]));
  }
}
typedef struct Msg_M2 {
  vector<std::string> s1;
  vector<std::string> s2;
  // Create th structure -- serialise side
  Msg_M2() = default;
  Msg_M2(int size_idx, int type) {
    if (MESSAGE_SIZES[size_idx] == 1 KB) {
      // total size = 1024
      if (type == 0) {
        // #members = 1
        vector<int> sizes{1024};
        FillString(s2, sizes);
      }
      if (type == 1) {
        // per member size = 512
        // distribute randomly
        vector<int> sizes{7, 209, 190, 87, 19}; // Adds to 512
        FillString(s1, sizes);
        FillString(s2, sizes);
      }
    } else if (MESSAGE_SIZES[size_idx] == 10 KB) {
      if (type == 0) {
        // #members = 1
        vector<int> sizes{10 KB};
        FillString(s2, sizes);
      }
      if (type == 1) {
        // per member size = 5KB = 5120
        // distribute randomly
        vector<int> sizes{7, 3010, 190, 207, 1029, 848}; // Adds to 512
        FillString(s1, sizes);
        FillString(s2, sizes);
      }
    } else if (MESSAGE_SIZES[size_idx] == 30 KB) {
      if (type == 0) {
        // #members = 1
        vector<int> sizes{30 KB};
        FillString(s2, sizes);
      }
      if (type == 1) {
        // per member size = 15KB = 15360
        // distribute randomly
        vector<int> sizes{14, 6020, 2800, 1808, 3448, 1270}; // Adds to 15KB
        FillString(s1, sizes);
        FillString(s2, sizes);
      }
    } else if (MESSAGE_SIZES[size_idx] == 100 KB) {
      if (type == 0) {
        // #members = 1
        vector<int> sizes{100 KB};
        FillString(s2, sizes);
      }
      if (type == 1) {
        // per member size = 50 KB = 51200
        // distribute randomly
        vector<int> sizes{7000, 3010, 40090, 100, 529, 471}; // Adds to 512
        FillString(s1, sizes);
        FillString(s2, sizes);
      }
    } else if (MESSAGE_SIZES[size_idx] == 256 KB) {
      if (type == 0) {
        // #members = 1
        vector<int> sizes{256 KB};
        FillString(s2, sizes);
      }
      if (type == 1) {
        // per member size = 128KB = 131072
        // distribute randomly
        vector<int> sizes{7,     78,    987,  40000,
                          30000, 20000, 40000}; // Adds to 512
        FillString(s1, sizes);
        FillString(s2, sizes);
      }
    } else {
      NOT_IMPLEMENTED;
    }
    /*
      s1.push_back("abcd");
      s1.push_back("efg");

      s2.push_back("qwerty");
      s2.push_back("poiuy");
      */
    Print();
  }
  void Print() {
    return;
    std::cout << "s1Len: " << s1.size() << " s2Len: " << s2.size() << std::endl;
    std::cout << "s1: [ ";
    for (auto s : s1) {
      std::cout << "\"" << s << "\" ";
    }
    std::cout << " ]\n";
    std::cout << "s2: [ ";
    for (auto s : s2) {
      std::cout << "\"" << s << "\" ";
    }
    std::cout << " ]\n";
  }
#ifdef PROTOBUF
  ssize_t SerialiseAndSendPB(TCPConn *conn) {
    Helper::time_unit_t alloc_time = 0, copy_time = 0, write_time = 0,
                        dealloc_time = 0, total_time = 0;
    ssize_t bytes = 0;
    {
      Helper::Timer<Helper::nanosecond_t> _(total_time);
      M2 *m2 = new M2();
      for (auto &s : s1) {
        m2->add_a1(s);
      }
      for (auto &s : s2) {
        m2->add_a2(s);
      }
      string ser_buf = "";
      assert(m2->SerializeToString(&ser_buf));
      std::cout << "PBSIZE: " << ser_buf.length() << std::endl;
      bytes = conn->Write(ser_buf.c_str(), ser_buf.length());
      std::cout << "PBSIZE: " << ser_buf.length() << " bytes: " << bytes << std::endl;
      delete m2;
    }
    //total_time += alloc_time + copy_time + write_time + dealloc_time;
    LatencyRecorderMgr::Get()->AddMetric(LatencyRecorderMgr::Metric(
        alloc_time, copy_time, write_time, dealloc_time, total_time));
    std::cout << "TOTAL TIME: " << total_time << std::endl;
    return bytes;
  }

  static Msg_M2 *CreateMsgFromPB(char *buf, size_t size) {
    M2 *de_msg = new M2();
    Msg_M2 *msg = new Msg_M2();

    assert(de_msg->ParseFromArray(buf, size));
    for (auto i = 0; i < de_msg->a1().size(); i++) {
      msg->s1.push_back(de_msg->a1(i));
    }
    for (auto i = 0; i < de_msg->a2().size(); i++) {
      msg->s2.push_back(de_msg->a2(i));
    }
    msg->Print();
    delete de_msg;
    return msg;
  }
#endif

#ifdef FLATBUF
  ssize_t SerialiseAndSendFB(TCPConn *conn) {
    Helper::time_unit_t alloc_time = 0, copy_time = 0, write_time = 0,
                        dealloc_time = 0, total_time = 0;
    // flatbuffers::FlatBufferBuilder builder;
    int bufferSize = 0;
    uint8_t *bufferPointer = nullptr;
    ssize_t bytes = 0;
    /*
    {Helper::Timer<Helper::nanosecond_t> _(copy_time);
    std::vector<flatbuffers::Offset<flatbuffers::String>> a1_vec;
    for(auto s : s1) {
      a1_vec.push_back(builder.CreateString(s));
    }
    std::vector<flatbuffers::Offset<flatbuffers::String>> a2_vec;
    for(auto s : s2) {
      a2_vec.push_back(builder.CreateString(s));
    }
    auto a1 = builder.CreateVector(a1_vec);
    auto a2 = builder.CreateVector(a2_vec);
    auto t2 = FBschema::CreateT2(builder, a1, a2);
    builder.Finish(t2);
     bufferPointer = builder.GetBufferPointer();
    bufferSize = builder.GetSize();
    }
    {Helper::Timer<Helper::nanosecond_t> _(write_time);
    bytes = conn->Write((char*)bufferPointer, bufferSize);
    }
    */

    flatbuffers::FlatBufferBuilder *builder = nullptr;
    {
      Helper::Timer<Helper::nanosecond_t> _(total_time);
      builder = new flatbuffers::FlatBufferBuilder();
      // Helper::Timer<Helper::nanosecond_t> _(copy_time);
      std::vector<flatbuffers::Offset<flatbuffers::String>> a1_vec;
      for (auto s : s1) {
        a1_vec.push_back(builder->CreateString(s));
      }
      std::vector<flatbuffers::Offset<flatbuffers::String>> a2_vec;
      for (auto s : s2) {
        a2_vec.push_back(builder->CreateString(s));
      }
      auto a1 = builder->CreateVector(a1_vec);
      auto a2 = builder->CreateVector(a2_vec);
      auto t2 = FBschema::CreateT2(*builder, a1, a2);
      builder->Finish(t2);
      bufferPointer = builder->GetBufferPointer();
      bufferSize = builder->GetSize();
      std::cout << "FBSIZE: " << bufferSize << std::endl;
      bytes = conn->Write((char *)bufferPointer, bufferSize);
      delete builder;
    }

    // total_time = alloc_time + copy_time + write_time + dealloc_time;
    LatencyRecorderMgr::Get()->AddMetric(LatencyRecorderMgr::Metric(
        alloc_time, copy_time, write_time, dealloc_time, total_time));
    std::cout << "TOTAL TIME: " << total_time << std::endl;
    return bytes;
  }
  static Msg_M2 *CreateMsgFromFB(char *buf, size_t size) {
    const uint8_t *buffer = (uint8_t *)buf;
    // flatbuffers::FlatBufferVerifier verifier(buffer, bufferSize); //
    // bufferSize is the size of the buffer if
    // (!FBschema::VerifyT2Buffer(verifier)) {
    //   std::cerr << "Error: Invalid buffer format" << std::endl;
    //   // return 1;
    // }
    Msg_M2 *msg = new Msg_M2();

    auto t2 = flatbuffers::GetRoot<FBschema::T2>(buffer);

    auto a1 = t2->a1();
    auto a2 = t2->a2();

    for (size_t i = 0; i < a1->size(); ++i) {
      msg->s1.push_back(a1->Get(i)->str());
    }

    for (size_t i = 0; i < a2->size(); ++i) {
      msg->s2.push_back(a2->Get(i)->str());
    }

    // msg->Print();
    return msg;
  }
#endif

  ssize_t SerialiseAndSend(TCPConn *conn,
                           SerializationMethod serialization_method) {
    Helper::time_unit_t total_time = 0;
    ssize_t bytes = 0;
    {
      Helper::Timer<Helper::nanosecond_t> _(total_time);
      user::Message *m1 = user::InitMessage::allocateMessage("M2");
      assert(m1 != nullptr);
      for (auto &s : s1) {
        m1->addVal("a1", s);
      }
      for (auto &s : s2) {
        m1->addVal("a2", s);
      }
      bytes = m1->serializeAndWrite(conn, serialization_method);
      cout << "Wrote " << bytes
           << " bytes. TYPE: " << SerializationMethodToStr(serialization_method)
           << std::endl;
      delete m1;
    }
    LatencyRecorderMgr::Get()->AddMetric(
        LatencyRecorderMgr::Metric(0, 0, 0, 0, total_time));
    std::cout << "TOTAL TIME: " << total_time << std::endl; 
    return bytes;
  }
  static Msg_M2 *CreateMsg(char *buf, size_t size) {
    Deserialiser *de_obj = InitMessage::getDeserialiserPtr("M2", buf, size);
    Msg_M2 *msg = new Msg_M2();

    std::optional<uint32_t> a1_size = 0, a2_size = 0;
    a1_size = de_obj->size("a1"), a2_size = de_obj->size("a2");
    // cout << "a1_size: " << (a1_size.has_value() ? to_string(*a1_size) :
    // "NULL")
    // << endl; cout << "a2_size: " << (a2_size.has_value() ?
    // to_string(*a2_size) : "NULL") << endl;

    // cout << "a1: [";
    for (uint32_t i = 0; a1_size.has_value() && i < *a1_size; i++) {
      std::optional<byte_t> val = de_obj->getByte("a1", i);
      // cout << "{ " << val->second << " \""
      //     << string(val->first, val->first + val->second) << "\""
      //     << " } ";
      msg->s1.push_back(string(val->first, val->first + val->second));
    }
    // cout << "]" << endl;
    // cout << "a2: [";
    for (uint32_t i = 0; a2_size.has_value() && i < *a2_size; i++) {
      std::optional<byte_t> val = de_obj->getByte("a2", i);
      // std::cout << "VAL:: " << val->second << std::endl;
      // cout << "{ " << val->second << " \""
      //     << string(val->first, val->first + val->second) << "\""
      //     << " } ";
      msg->s2.push_back(string(val->first, val->first + val->second));
    }
    // cout << "]" << endl;
    delete de_obj;
    return msg;
  }
} Msg_M2;

void initUser() {
  SymTab::init();
  SymTab::readFile("proto/synthetic.proto");
}
#endif
