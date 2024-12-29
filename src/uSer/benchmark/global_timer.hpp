#pragma once

#include "common.hpp"
#include "helper.hpp"

using Helper::time_unit_t;

template <typename T> class GlobalTimerMgr {
public:
  static GlobalTimerMgr *GetInstance() { return instance_; }
  static void StartTimer(time_unit_t interval) {
    instance_ = new GlobalTimerMgr(Helper::Timer<T>::CurrentTime(), interval);
  }
  auto IsOver() -> bool {
    // auto curr = Helper::Timer<T>::CurrentTime();
    // std::cout << curr << " " << start_time_ << " " << interval_ << std::endl;
    // return ((curr - start_time_) >= interval_);
    return ((Helper::Timer<T>::CurrentTime() - start_time_) >= interval_);
  }
  void UpdateInterval(time_unit_t interval) { interval_ = interval; }
  void UpdateIntervalBy(time_unit_t interval) {
    interval_ += interval;
    // std::cout << "Updated time by " << interval << " " << start_time_ << "
    // NewTime: " << interval_
    //          << std::endl;
  }
  static GlobalTimerMgr *instance_;
  time_unit_t start_time_{0};
  time_unit_t interval_{0};

private:
  GlobalTimerMgr() = delete;
  GlobalTimerMgr(time_unit_t start_time, time_unit_t interval)
      : start_time_(start_time), interval_(interval) {}
};
template <typename T> GlobalTimerMgr<T> *GlobalTimerMgr<T>::instance_ = nullptr;
