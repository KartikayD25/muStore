/*
 * Latency Recorder
 */
#pragma once

#include <fstream>

#include "helper.hpp"

class LatencyRecorderMgr {
public:
  LatencyRecorderMgr(const LatencyRecorderMgr &) = delete;

  static LatencyRecorderMgr *Init() {
    if (latencyRecorderMgr_ == nullptr) {
      latencyRecorderMgr_ = new LatencyRecorderMgr();
    }
    return latencyRecorderMgr_;
  }
  static LatencyRecorderMgr *Get() { return latencyRecorderMgr_; }
  void StartNewStats(int type, serialization_method_t method, size_t size,
                     size_t num_fields) {
    stats_.push_back(Stats(type, method, size, num_fields));
  }
  void DumpLastLatencyRecorderMgrStat() {
    stats_.back().DumpReadable();
    std::cout << std::endl;
  }
  void DumpStatReadable() {
    for (auto &stat : stats_) {
      std::cout << stat.DumpReadable();
    }
  }
  void DumpStatToCSV(std::string &file_name) {
    std::ofstream file(file_name);
    file << Stats::DumpAggregatedStatHeaderCSV() << std::endl;
    for (auto &stat : stats_) {
      file << stat.DumpAggregatedStatCSV() << std::endl;
    }
    file.close();
  }
  void DumpAllMetricsToCSV(const std::string &file_name) {
    std::ofstream file(file_name);
    
    // Write CSV header
    file << "method,type,bytes,num_fields,prep_time,copy_time,alloc_time,dealloc_time,write_time,total_time" << std::endl;

    for (const auto &stat : stats_) {
        for (const auto &metric : stat.metric_) {
            // Write each metric to the CSV file
            file << SerializationMethodToStr(stat.method_) << ","
                 << stat.type_ << ","
                 << stat.bytes_ << ","
                 << stat.num_fields_ << ","
                 << metric.prep_time_ << ","
                 << metric.copy_time_ << ","
                 << metric.alloc_time_ << ","
                 << metric.dealloc_time_ << ","
                 << metric.write_time_ << ","
                 << metric.total_time_ << std::endl;
        }
    }
    
    file.close();
}
  void DumpAggregatedStat() {
    for (auto &stat : stats_) {
      std::cout << "LATENCY(ns) " << stat.DumpAggregatedStatReadable()
                << std::endl;
    }
  }
  class Metric {
  public:
    double prep_time_ = 0, copy_time_ = 0;
    double total_time_ = 0, write_time_ = 0;
    double alloc_time_ = 0, dealloc_time_ = 0;
    Metric(Helper::time_unit_t prep_time, Helper::time_unit_t write_time,
           Helper::time_unit_t total_time)
        : prep_time_(prep_time), total_time_(total_time),
          write_time_(write_time) {}
    Metric(Helper::time_unit_t alloc_time, Helper::time_unit_t copy_time,
           Helper::time_unit_t write_time, Helper::time_unit_t dealloc_time,
           Helper::time_unit_t total_time)
        : copy_time_(copy_time), total_time_(total_time),
          write_time_(write_time), alloc_time_(alloc_time),
          dealloc_time_(dealloc_time) {}
  };
  void AddMetric(Metric metric) { stats_.back().AddMetric(metric); }

private:
  class Stats {
  public:
    const int type_;
    const serialization_method_t method_;
    const size_t bytes_;
    const size_t num_fields_;
    std::vector<Metric> metric_;
    double avg_alloc_time_ = 0, avg_dealloc_time_ = 0;
    double avg_copy_time_ = 0, avg_prep_time_ = 0;
    float avg_write_time_ = 0, avg_total_time_ = 0;
    Stats(int type, serialization_method_t method, size_t bytes,
          size_t num_fields)
        : type_(type), method_(method), bytes_(bytes), num_fields_(num_fields) {
    }
    void AddMetric(Metric metric) { metric_.push_back(metric); }
    void Aggregate() {
      avg_alloc_time_ = 0;
      avg_dealloc_time_ = 0;
      avg_copy_time_ = 0;
      avg_prep_time_ = 0;
      avg_write_time_ = 0;
      avg_total_time_ = 0;
      if (metric_.empty())
        return;
      for (auto &m : metric_) {
        avg_alloc_time_ += m.alloc_time_;
        avg_dealloc_time_ += m.dealloc_time_;
        avg_copy_time_ += m.copy_time_;
        avg_prep_time_ += m.prep_time_;
        avg_write_time_ += m.write_time_;
        avg_total_time_ += m.total_time_;
      }
      avg_alloc_time_ /= metric_.size();
      avg_dealloc_time_ /= metric_.size();
      avg_copy_time_ /= metric_.size();
      avg_prep_time_ /= metric_.size();
      avg_write_time_ /= metric_.size();
      avg_total_time_ /= metric_.size();
    }
    std::string DumpReadable() {
      std::stringstream ss;
      for (auto &m : metric_) {
        if (method_ == serialization_method_t::zc) {
          ss << std::fixed << std::setprecision(3) << "SER_SG"
             << " type " << type_ << " bytes " << bytes_ << " fields "
             << num_fields_ << " prep_time " << m.prep_time_ << " write_time "
             << m.write_time_ << " total_time " << m.total_time_ << std::endl;
        } else {
          ss << std::fixed << std::setprecision(3) << "SER_COPY"
             << " type " << type_ << " bytes " << bytes_ << " fields "
             << num_fields_ << " alloc_time " << m.alloc_time_ << " copy_time "
             << m.copy_time_ << " dealloc_time " << m.dealloc_time_
             << " write_time " << m.write_time_ << " total_time "
             << m.total_time_ << std::endl;
        }
      }
      return ss.str();
    }
    std::string DumpAggregatedStatReadable() {
      Aggregate();
      std::stringstream ss;
      ss << std::fixed << std::setprecision(3) << " Method "
         << SerializationMethodToStr(method_) << " type " << type_ << " size "
         << bytes_ << " numFields " << num_fields_ << " avgAlloc "
         << avg_alloc_time_ << " avgDealloc " << avg_dealloc_time_
         << " avgCopy " << avg_copy_time_ << " avgPrep " << avg_prep_time_
         << " avgWrite " << avg_write_time_ << " avgTotal " << avg_total_time_
         << " rounds " << metric_.size();
      return ss.str();
    }
    std::string DumpAggregatedStatCSV() {
      Aggregate();
      std::stringstream ss;
      std::cout<< SerializationMethodToStr(method_);
      ss << std::fixed << std::setprecision(3)
         << SerializationMethodToStr(method_) << "," << type_ << "," << bytes_
         << "," << num_fields_ << "," << avg_alloc_time_ << ","
         << avg_dealloc_time_ << "," << avg_copy_time_ << "," << avg_prep_time_
         << "," << avg_write_time_ << "," << avg_total_time_;
      return ss.str();
    }
    static std::string DumpAggregatedStatHeaderCSV() {
      std::stringstream ss;
      ss << "method"
         << ","
         << "type"
         << ","
         << "bytes"
         << ","
         << "num_fields"
         << ","
         << "avg_alloc_time"
         << ","
         << "avg_dealloc_time"
         << ","
         << "avg_copy_time"
         << ","
         << "avg_prep_time"
         << ","
         << "avg_write_time"
         << ","
         << "avg_total_time";
      return ss.str();
    }
  };
  static LatencyRecorderMgr *latencyRecorderMgr_;
  std::vector<Stats> stats_;
  LatencyRecorderMgr() = default;
};

LatencyRecorderMgr *LatencyRecorderMgr::latencyRecorderMgr_ = nullptr;
