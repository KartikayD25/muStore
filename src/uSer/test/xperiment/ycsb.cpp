#include "user.hpp"
#include <thread>
#include "helper.hpp"
#include "experiment.h"
#include "Person.hpp"
#include <vector>
#include <chrono>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <numeric>
#include <linux/perf_event.h>    // For perf_event_attr
#include <sys/ioctl.h>           // For ioctl
#include <unistd.h>              // For read, close
#include <sys/syscall.h> 
#include "message.hpp"
#include <functional>

#include "message.hpp"
#include <atomic>
using namespace std;
using namespace user;
using namespace Helper;
net::common::Address server_address(SERVER_IP, SERVER_PORT);

char * getStr(int size){
  char * str = (char *) calloc(1,size);
  Random::FillRandomReadableASCII(str,size);
  return str;
}


// Wrapper for the perf_event_open syscall
static long perf_event_open(struct perf_event_attr* hw_event,
                          pid_t pid,
                          int cpu, 
                          int group_fd,
                          unsigned long flags) {
    int ret = syscall(__NR_perf_event_open, hw_event, pid, cpu, group_fd, flags);
    return ret;
}

class SerializationBenchmark {

public:
    // SerializationBenchmark(): connection_(nullptr){};
    SerializationBenchmark(user::net::TCPClient * conn): connection_(conn){}

private:
    // Performance metrics
    struct Metrics {
        std::vector<double> allocation_times;
        std::vector<double> serialization_times;
        std::vector<double> sending_times;
        std::vector<size_t> memory_sizes;
        std::vector<uint64_t> cache_misses;
    };

    // For memory tracking
    struct MemorySnapshot {
        size_t virtual_memory;
        size_t resident_memory;
        size_t peak_memory;
    };

    std::atomic<bool> is_running_{true};
    std::vector<MemorySnapshot> memory_snapshots_;
    user::net::Conn * connection_;  // Need to store connection instance

    // Add missing utility functions
    size_t parseKBytes(const std::string& line) {
        size_t kb;
        std::istringstream iss(line.substr(line.find(':') + 1));
        iss >> kb;
        return kb;
    }

    // size_t getMessageMemorySize(Message* msg) {
    //     // Implement based on your Message class
    //     return msg->getSize();  // Or however you measure message size
    // }


    double calculateAverageTime(std::vector<Metrics>& metrics,
        std::function<const std::vector<double>&(Metrics&)> getter) {
        double sum = 0;
        size_t count = 0;
        for (auto& m : metrics) {
            auto& times = getter(m);
            sum += std::accumulate(times.begin(), times.end(), 0.0);
            count += times.size();
        }
        double ans =  count > 0 ? sum / count : 0;
        std::cout<<ans;
        return ans;
    }

    void generateReport(const std::vector<Metrics>& metrics) {
        // Implement detailed report generation
        std::ofstream report("benchmark_report.txt");
        report << "Detailed Performance Report\n";
        // Add your report generation logic
    }

public:
    void runTest(int num_threads, int messages_per_thread, size_t message_size) {
        std::vector<std::thread> threads;
        std::vector<Metrics> thread_metrics(num_threads);
        
        // Setup hardware performance counters for cache misses
        struct perf_event_attr pe;
        memset(&pe, 0, sizeof(pe));
        pe.type = PERF_TYPE_HARDWARE;
        pe.config = PERF_COUNT_HW_CACHE_MISSES;
        pe.config = (PERF_COUNT_HW_CACHE_LL) |
            (PERF_COUNT_HW_CACHE_OP_READ << 8) |
            (PERF_COUNT_HW_CACHE_RESULT_MISS << 16);
        pe.disabled = 1;
        pe.exclude_kernel = 1;
        
        // Create threads
        for (int i = 0; i < num_threads; i++) {
            threads.emplace_back([this, i, messages_per_thread, message_size, &thread_metrics, &pe]() {
                this->threadFunction(i, messages_per_thread, message_size, thread_metrics[i], pe);
            });
        }

        // Monitor system memory usage while test runs
        std::thread memory_monitor([this]() {
            while (is_running_) {
                memory_snapshots_.push_back(takeMemorySnapshot());
                std::this_thread::sleep_for(std::chrono::nanoseconds(100));
            }
        });
        std::cout<<"HELLO";
        // Wait for completion
        for (auto& thread : threads) {
            thread.join();
            std::cout<<"THread complete";
        }
        //std::cout<<"hello";
        is_running_ = false;
        memory_monitor.join();
        //std::cout<<"HELLo";
        // Analyze and report results
        analyzeResults(thread_metrics);
    }

private:
    void threadFunction(int thread_id, int message_count, size_t message_size, 
                       Metrics& metrics, const perf_event_attr& pe) {
        // Setup performance counter for this thread
        struct perf_event_attr pe_local = pe;
        int fd = perf_event_open(&pe_local, 0, -1, -1, 0);
        ioctl(fd, PERF_EVENT_IOC_RESET, 0);
        ioctl(fd, PERF_EVENT_IOC_ENABLE, 0);

        for (int i = 0; i < message_count; i++) {
            // Measure allocation time
            auto start = std::chrono::high_resolution_clock::now();
            // user::Message* msg = user::InitMessage::allocateMessage("TestMessage");
            Person * p = new Person();
            auto alloc_end = std::chrono::high_resolution_clock::now();
            string s1,s2,s3,s4,s5,s6,s7,s8,s9,s10,s11;
            //Fill data in TestMessage
            if(message_size==1024){

              //std::cout<<"Hello";
              char* f1 = getStr(100);
              s1 = f1;
              char* f2 = getStr(100); 
              s2 = f2;
              char* f3 = getStr(100);
              s3 = f3;
              char* f4 = getStr(100);
              s4 = f4;
              char* f5 = getStr(100);
              s5 = f5;
              char* f6 = getStr(100);
              s6 = f6;
              char* f7 = getStr(100);
              s7 = f7;
              char* f8 = getStr(100);
              s8 = f8;
              char* f9 = getStr(100);
              s9 = f9;
              char* f10 = getStr(100);
              s10 = f10;
              char* f11 = getStr(24);
              s11 = f11;   
            }else if (message_size==10240){
              char* f1 = getStr(1000);
              s1 = f1;
              char* f2 = getStr(1000); 
              s2 = f2;
              char* f3 = getStr(1000);
              s3 = f3;
              char* f4 = getStr(1000);
              s4 = f4;
              char* f5 = getStr(1000);
              s5 = f5;
              char* f6 = getStr(1000);
              s6 = f6;
              char* f7 = getStr(1000);
              s7 = f7;
              char* f8 = getStr(1000);
              s8 = f8;
              char* f9 = getStr(1000);
              s9 = f9;
              char* f10 = getStr(1000);
              s10 = f10;
              char* f11 = getStr(240);
              s11 = f11;   
            }else{
              char* f1 = getStr(10000);
              s1 = f1;
              char* f2 = getStr(10000); 
              s2 = f2;
              char* f3 = getStr(10000);
              s3 = f3;
              char* f4 = getStr(10000);
              s4 = f4;
              char* f5 = getStr(10000);
              s5 = f5;
              char* f6 = getStr(10000);
              s6 = f6;
              char* f7 = getStr(10000);
              s7 = f7;
              char* f8 = getStr(10000);
              s8 = f8;
              char* f9 = getStr(10000);
              s9 = f9;
              char* f10 = getStr(10000);
              s10 = f10;
              char* f11 = getStr(2400);
              s11 = f11;   
            }
            // Measure serialization and sending time

            p->set_field1(s1);
            p->set_field2(s2);
            p->set_field3(s3);
            p->set_field4(s4);
            p->set_field5(s5);
            p->set_field6(s6);
            p->set_field7(s7);
            p->set_field8(s8);
            p->set_field9(s9);
            p->set_field10(s10);
            p->set_field11(s11);
            //std::cout<<"HEllo";
            auto ser_start = std::chrono::high_resolution_clock::now();
            p->serializeAndWrite(connection_,user::SerializationMethod::ZC);
            auto send_end = std::chrono::high_resolution_clock::now();

            // Record metrics
            metrics.allocation_times.push_back(
                std::chrono::duration<double>(alloc_end - start).count());
            metrics.serialization_times.push_back(
                std::chrono::duration<double>(send_end - ser_start).count());
            size_t size = 1024;
            metrics.memory_sizes.push_back(size);

            // Read cache misses
            uint64_t cache_misses;
            read(fd, &cache_misses, sizeof(cache_misses));
            metrics.cache_misses.push_back(cache_misses);
        }

        close(fd);
        return;
    }

    MemorySnapshot takeMemorySnapshot() {
        MemorySnapshot snapshot;
        std::ifstream status("/proc/self/status");
        std::string line;
        
        while (std::getline(status, line)) {
            if (line.substr(0, 7) == "VmSize:")
                snapshot.virtual_memory = parseKBytes(line);
            else if (line.substr(0, 6) == "VmRSS:")
                snapshot.resident_memory = parseKBytes(line);
            else if (line.substr(0, 6) == "VmHWM:")
                snapshot.peak_memory = parseKBytes(line);
        }
        
        return snapshot;
    }
    void analyzeResults(std::vector<Metrics>& thread_metrics) {
        std::cout<< thread_metrics.size();
        for(int i =0;i<thread_metrics[0].serialization_times.size();i++){
            std::cout<<thread_metrics[0].serialization_times[i]<<'\n';
        }
        std::cout<<"Cache misses";
        for(int i =0;i<thread_metrics[0].cache_misses.size();i++){
            std::cout<<thread_metrics[0].cache_misses[i]<<'\n';
        }
        std::cout<<"Peak memories";
        for(auto i:memory_snapshots_){
            std::cout<< i.peak_memory;
        }
        // Calculate statistics
        // std::cout << thread_metrics.size();
        // // Allocation time stats
        // double avg_alloc_time = calculateAverageTime(thread_metrics, 
        //     [](const Metrics& m) { return m.allocation_times; });
        // std::cout<<"HELLO";
        // std::cout << "Average allocation time: " << avg_alloc_time << " ms\n";

        // // Memory usage stats
        // size_t peak_memory = std::max_element(memory_snapshots_.begin(), 
        //     memory_snapshots_.end(),
        //     [](const auto& a, const auto& b) { 
        //         return a.resident_memory < b.resident_memory; 
        //     })->resident_memory;
        // std::cout << "Peak memory usage: " << peak_memory / 1024 << " MB\n";

        // // Cache performance
        // uint64_t total_cache_misses = 0;
        // for (const auto& metrics : thread_metrics) {
        //     total_cache_misses += std::accumulate(metrics.cache_misses.begin(), 
        //                                         metrics.cache_misses.end(), 0ULL);
        // }
        // std::cout << "Total cache misses: " << total_cache_misses << "\n";


        // Generate detailed report
     //   generateReport(thread_metrics);
    }
};









void server() {
  net::TCPServer server(server_address, 5);
  assert(server.StartListen());
  net::TCPConn *conn = server.GetNewConn();
  std::cout<<"Listening";
  size_t size = user::net::common::BUFFER_SIZE;
  char *buf = (char *)malloc(size);

  // while (true) {
  //   bzero(buf, size);
  //   cout << "Waiting for read..\n";
  //   ssize_t bytes = conn->Read(buf, size);
  //   cout << "Read " << bytes << std::endl;

  //   // Deserialise the message as VideoMetadata
  //   Deserialiser *de_obj =
  //       InitMessage::getDeserialiserPtr("Geo", buf, size);

  //   // Retrieve field sizes (if they are arrays)
  //   std::optional<int> lat = 0, lon = 0;
  //   lat = de_obj->get<float>("lat");
  //   lon = de_obj->get<float>("long");
 

  //   // Print sizes of arrays (or single fields)
  //   cout << "lat: " << (lat.has_value() ? to_string(*lat) : "NULL") << endl;
  //   cout << "lon: " << (lon.has_value() ? to_string(*lon) : "NULL") << endl;

  //   break;
  // }

  free(buf);  // Don't forget to free the allocated memory
}
void client() {
  // ssize_t bytes = 0;
  net::TCPClient conn(server_address);
  // cout << "Connecting...\n";
  assert(conn.Connect());
  cout << "Connection Done\n";
  /*
  {
    user::Message *m1 = user::InitMessage::allocateMessage("Person");
    assert(m1 != nullptr);
    int age = 26;
    m1->setVal("name", "Siddharth");
    m1->setVal("age", age);

    auto bytes = m1->serializeAndWrite(&conn, SerializationMethod::COPY);
    cout << "Wrote " << bytes << " bytes." << std::endl;
  }
  */
  {
      char* f1 = getStr(100);
      string s1 = f1;
      char* f2 = getStr(100); 
      string s2 = f2;
      char* f3 = getStr(100);
      string s3 = f3;
      char* f4 = getStr(100);
      string s4 = f4;
      char* f5 = getStr(100);
      string s5 = f5;
      char* f6 = getStr(100);
      string s6 = f6;
      char* f7 = getStr(100);
      string s7 = f7;
      char* f8 = getStr(100);
      string s8 = f8;
      char* f9 = getStr(100);
      string s9 = f9;
      char* f10 = getStr(100);
      string s10 = f10;
      char* f11 = getStr(24);
      string s11 = f11;   
    LatencyRecorderMgr::Init()->StartNewStats(0,serialization_method_t::zc , 11, 11);
    for (int i = 0 ; i < 10 ; i++){
      Person * p = new Person();
      //assert(m2 != nullptr);
      p->set_field1(s1);
      p->set_field1(s2);
      p->set_field1(s3);
      p->set_field1(s4);
      p->set_field1(s5);
      p->set_field1(s6);
      p->set_field1(s7);
      p->set_field1(s8);
      p->set_field1(s9);
      p->set_field1(s10);
      p->set_field1(s11);
      auto bytes = p->serializeAndWrite(&conn, SerializationMethod::SG);
      cout << "Wrote " << bytes << " bytes." << std::endl;
    } 
    std::string filename = "ycsb_all.csv";
    std::string filename_agg = "ycsb_agg.csv";
    LatencyRecorderMgr::Get()->DumpAllMetricsToCSV(filename);
    LatencyRecorderMgr::Get()->DumpStatToCSV(filename_agg);
  }
}

int main() {
  std::cout << "PID " << getpid() << std::endl;
  /****** INIT uSser *********/
  SymTab::init();
  // /***************************/

  // /***** Read Proto files *****/
  SymTab::readFile("../proto/final.proto");
  /***************************/
  thread _(server);
  sleep(2);
    net::TCPClient conn(server_address);
  // cout << "Connecting...\n";
  assert(conn.Connect());
  SerializationBenchmark benchmark(&conn);

  cout << "Connection Done\n";    
    // Test different scenarios
    std::vector<size_t> message_sizes = {1024}; // Different sizes
    std::vector<int> thread_counts = {1,10}; // Different parallel loads
    
    for (size_t size : message_sizes) {
        for (int threads : thread_counts) {
            std::cout << "\nTesting with " << threads << " threads, "
                      << "message size: " << size << " bytes\n";
            benchmark.runTest(threads, 100, size);
            
        }
    }

//  client();

  return 0;
}
// int main() {
//     SerializationBenchmark benchmark;
    
//     // Test different scenarios
//     std::vector<size_t> message_sizes = {1024, 10240, 102400}; // Different sizes
//     std::vector<int> thread_counts = {1, 2, 4, 8, 16}; // Different parallel loads
    
//     for (size_t size : message_sizes) {
//         for (int threads : thread_counts) {
//             std::cout << "\nTesting with " << threads << " threads, "
//                       << "message size: " << size << " bytes\n";
//             benchmark.runTest(threads, 1000, size);
//         }
//     }
// }