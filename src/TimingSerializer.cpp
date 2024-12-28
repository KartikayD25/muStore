#include "TimingSerializer.hpp"
#include <chrono>
#include "Logger.hpp"
#include "papi.h"
#include <sys/resource.h>
#include <mutex>

std::mutex mtx;
std::mutex papi_mtx;

TimingSerializer::TimingSerializer(std::unique_ptr<ISerializer> base, const std::string& csvPath) 
    : baseSerializer(std::move(base)) {
    std::lock_guard<std::mutex> lock(mtx);
    csvFile.open(csvPath, std::ios::app);
    if (!csvFile.tellp()) {
        csvFile << "timestamp,serializer_type,data_size_bytes,duration_ns,cache_misses,branch_misses,cpu_cycles,max_memory_kb\n";
    }
}

int TimingSerializer::serialize(const Response& msg) {
    // Initialize PAPI
    std::string serializerType;
    long long cacheMisses = -1;
    long long branchMisses = -1;
    long long cpuCycles = -1;
    int eventSet = PAPI_NULL;
    int result = 0;
    long long counters[3];
    struct rusage usage;
    long long maxMemory;
    std::chrono::_V2::system_clock::rep duration;
    std::chrono::_V2::system_clock::time_point start, end;

    {
        std::lock_guard<std::mutex> lock(papi_mtx);
        if (PAPI_library_init(PAPI_VER_CURRENT) != PAPI_VER_CURRENT) {
            LOG_ERROR("PAPI library initialization error!");
            return baseSerializer->serialize(msg);
        }

        if (PAPI_create_eventset(&eventSet) != PAPI_OK) {
            LOG_ERROR("PAPI create event set error!");
            return baseSerializer->serialize(msg);
        }

        if (PAPI_query_event(PAPI_TOT_CYC) == PAPI_OK) {
            if (PAPI_add_event(eventSet, PAPI_TOT_CYC) != PAPI_OK) {
                LOG_ERROR("PAPI add event PAPI_TOT_CYC error!");
                goto error;
            }
        } else {
            LOG_ERROR("PAPI event PAPI_TOT_CYC not supported!");
        }

        if (PAPI_query_event(PAPI_L3_TCM) == PAPI_OK) {
            if (PAPI_add_event(eventSet, PAPI_L3_TCM) != PAPI_OK) {
                LOG_ERROR("PAPI add event PAPI_L3_TCM error!");
                goto error;
            }
        } else {
            LOG_ERROR("PAPI event PAPI_L3_TCM not supported!");
        }

        if (PAPI_query_event(PAPI_BR_MSP) == PAPI_OK) {
            if (PAPI_add_event(eventSet, PAPI_BR_MSP) != PAPI_OK) {
                LOG_ERROR("PAPI add event PAPI_BR_MSP error!");
                goto error;
            }
        } else {
            LOG_ERROR("PAPI event PAPI_BR_MSP not supported!");
        }

        if (PAPI_start(eventSet) != PAPI_OK) {
            LOG_ERROR("PAPI start counters error!");
            goto error;
        }
    }

    start = std::chrono::high_resolution_clock::now();
    result = baseSerializer->serialize(msg);
    end = std::chrono::high_resolution_clock::now();

    {
        std::lock_guard<std::mutex> lock(papi_mtx);
        if (PAPI_stop(eventSet, counters) != PAPI_OK) {
            LOG_ERROR("PAPI stop counters error!");
            return result;
        }
    }

    duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    if (cpuCycles == 0) cpuCycles = counters[0];
    if (cacheMisses == 0) cacheMisses = counters[1];
    if (branchMisses == 0) branchMisses = counters[2];

    // Get memory usage
    getrusage(RUSAGE_SELF, &usage);
    maxMemory = usage.ru_maxrss; // in kilobytes

    switch (baseSerializer->getSendMethod()) {
        case SendMethod::TWOCOPY:
            serializerType = "TwoCopy";
            break;
        case SendMethod::ONECOPY:
            serializerType = "OneCopy";
            break;
        case SendMethod::ZEROCOPY:
            serializerType = "ZeroCopy";
            break;
        case SendMethod::MuSer:
            serializerType = "MuSer";
            break;
    }
    logTiming(serializerType, result, duration, cacheMisses, branchMisses, cpuCycles, maxMemory);
    return result;

error:
    {
        std::lock_guard<std::mutex> lock(papi_mtx);
        PAPI_stop(eventSet, counters);
    }
    return baseSerializer->serialize(msg);
}

void TimingSerializer::logTiming(const std::string& serializerType, size_t dataSize, long long duration, long long cacheMisses, long long branchMisses, long long cpuCycles, long long maxMemory) {
    std::lock_guard<std::mutex> lock(mtx);
    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()).count();

    csvFile << timestamp << ","
            << serializerType << ","
            << dataSize << ","
            << duration << ","
            << cacheMisses << ","
            << branchMisses << ","
            << cpuCycles << ","
            << maxMemory << "\n";
    csvFile.flush();
}