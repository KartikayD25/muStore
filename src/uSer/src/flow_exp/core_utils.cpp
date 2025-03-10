#pragma once
#include <pthread.h>
#include <sched.h>
#include <thread>
#include <stdexcept>

class CoreAffinity {
public:
    static void pin_to_core(int core_id) {
        cpu_set_t cpuset;
        CPU_ZERO(&cpuset);
        CPU_SET(core_id, &cpuset);
        
        int rc = pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
        if (rc != 0) {
            throw std::runtime_error("Failed to set thread affinity");
        }
    }

    static int get_core_count() {
        return std::thread::hardware_concurrency();
    }
};