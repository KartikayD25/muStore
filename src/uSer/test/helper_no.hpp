#ifndef __HELPER_HPP__
#define __HELPER_HPP__

#include <cassert>
#include <chrono>
#include <cstring>
#include <stdlib.h>

#include <cstdlib>
#include <iomanip> // put_time
#include <iostream>
#include <sstream>

#define CURR_READABLE_TIME Helper::GetCurrentReadableTime()

enum class serialization_method_t { copy, sg ,zc};

std::string SerializationMethodToStr(serialization_method_t method) {
  if(method == serialization_method_t::copy){
    return "COPY";
  }else if(method == serialization_method_t::sg){
    return "SG";
  }else {
    return "ZC";
  }
}

namespace Helper {

std::string GetCurrentReadableTime() {
  auto now = std::chrono::system_clock::now();
  auto in_time_t = std::chrono::system_clock::to_time_t(now);

  std::stringstream ss;
  ss << std::put_time(std::localtime(&in_time_t), "[%Y-%m-%d::%H-%M-%S]");
  return ss.str();
}

std::string GetEnv(std::string env_var) {
  char *env_p = nullptr;
  if ((env_p = std::getenv(env_var.c_str())) == nullptr) {
    std::cerr << "\"" << env_var << "\" is not set" << std::endl;
    return "";
  }
  return std::string(env_p);
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

class Random {
public:
  static inline int GetRandrange(int min, int max) {
    return rand() % (max - min + 1) + min;
  }
  static void FillRandomReadableASCII(char *mem, size_t size) {
    for (size_t i = 0; i < size - 1; i++) {
      // choose between 3 section 0: [a-z] 1: [A-Z] 2: [0-9]
      int section = GetRandrange(0, 2);
      int low, high;
      switch (section) {
      case 0:
        low = 'a';
        high = 'z';
        break;
      case 1:
        low = 'A';
        high = 'Z';
        break;
      case 2:
        low = '0';
        high = '9';
        break;
      default:
        std::cerr << __FILE__ << ":" << __LINE__ << " :: Invalid section"
                  << std::endl;
        abort();
      }
      // generate a random byte
      int x = GetRandrange(low, high);
      mem[i] = x;
    }
  }
};

using time_unit_t = long long;
using microsecond_t = std::chrono::microseconds;
using nanosecond_t = std::chrono::nanoseconds;
using second_t = std::chrono::seconds;

template <typename T> class Timer {
public:
  Timer() = delete;
  inline Timer(time_unit_t &st) : store_at(st) {
    t1 = std::chrono::high_resolution_clock::now();
  }
  inline ~Timer() {
    t2 = std::chrono::high_resolution_clock::now();
    auto elapsed = t2 - t1;
    store_at = std::chrono::duration_cast<T>(elapsed).count();
  }

  static inline time_unit_t CurrentTime() {
    return std::chrono::duration_cast<T>(
               std::chrono::system_clock::now().time_since_epoch())
        .count();
  }

private:
  time_unit_t &store_at;
  std::chrono::high_resolution_clock::time_point t1, t2;
};

class Allocator {
public:
  template <typename T> inline static T *Alloc(size_t bytes) {
    T *mem = static_cast<T *>(malloc(bytes));
    assert(mem != nullptr);
    return mem;
  }
  template <typename T> inline static T *AllocZeroed(size_t bytes) {
    // T *mem = Alloc<T>(bytes);
    // bzero(mem, bytes);
    // calloc mallocs then call bzero
    void *mem = calloc(bytes, 1);
    return static_cast<T *>(mem);
  }
};
} // namespace Helper
#endif
