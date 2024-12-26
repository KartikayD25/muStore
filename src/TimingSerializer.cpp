#include "TimingSerializer.hpp"
#include <chrono>
#include "Logger.hpp"
#include "papi.h"

TimingSerializer::TimingSerializer(std::unique_ptr<ISerializer> base, const std::string& csvPath) 
    : baseSerializer(std::move(base)) {
    csvFile.open(csvPath, std::ios::app);
    if (!csvFile.tellp()) {
        csvFile << "timestamp,serializer_type,data_size_bytes,duration_ns,cache_misses,branch_misses,cpu_cycles\n";
    }
}

int TimingSerializer::serialize(const Response& msg) {
    // Initialize PAPI
    if (PAPI_library_init(PAPI_VER_CURRENT) != PAPI_VER_CURRENT) {
        LOG_ERROR("PAPI library initialization error!");
        return -1;
    }

    int eventSet = PAPI_NULL;
    if (PAPI_create_eventset(&eventSet) != PAPI_OK) {
        LOG_ERROR("PAPI create event set error!");
        return -1;
    }

    long long cacheMisses = -1;
    long long branchMisses = -1;
    long long cpuCycles = -1;

    if (PAPI_query_event(PAPI_TOT_CYC) == PAPI_OK) {
        if (PAPI_add_event(eventSet, PAPI_TOT_CYC) == PAPI_OK) {
            cpuCycles = 0;
        } else {
            LOG_ERROR("PAPI add event PAPI_TOT_CYC error!");
        }
    } else {
        LOG_ERROR("PAPI event PAPI_TOT_CYC not supported!");
    }

    if (PAPI_query_event(PAPI_L3_TCM) == PAPI_OK) {
        if (PAPI_add_event(eventSet, PAPI_L3_TCM) == PAPI_OK) {
            cacheMisses = 0;
        } else {
            LOG_ERROR("PAPI add event PAPI_L3_TCM error!");
        }
    } else {
        LOG_ERROR("PAPI event PAPI_L3_TCM not supported!");
    }

    if (PAPI_query_event(PAPI_BR_MSP) == PAPI_OK) {
        if (PAPI_add_event(eventSet, PAPI_BR_MSP) == PAPI_OK) {
            branchMisses = 0;
        } else {
            LOG_ERROR("PAPI add event PAPI_BR_MSP error!");
        }
    } else {
        LOG_ERROR("PAPI event PAPI_BR_MSP not supported!");
    }

    // Start counting
    if (PAPI_start(eventSet) != PAPI_OK) {
        LOG_ERROR("PAPI start counters error!");
        return -1;
    }

    auto start = std::chrono::high_resolution_clock::now();
    int result = baseSerializer->serialize(msg);
    auto end = std::chrono::high_resolution_clock::now();

    // Stop counting
    long long counters[3];
    if (PAPI_stop(eventSet, counters) != PAPI_OK) {
        LOG_ERROR("PAPI stop counters error!");
        return -1;
    }

    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    if (cpuCycles == 0) cpuCycles = counters[0];
    if (cacheMisses == 0) cacheMisses = counters[1];
    if (branchMisses == 0) branchMisses = counters[2];

    std::string serializerType;
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
    logTiming(serializerType, result, duration, cacheMisses, branchMisses, cpuCycles);

    return result;
}

void TimingSerializer::logTiming(const std::string& serializerType, size_t dataSize, long long duration, long long cacheMisses, long long branchMisses, long long cpuCycles) {
    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()).count();

    csvFile << timestamp << ","
            << serializerType << ","
            << dataSize << ","
            << duration << ","
            << cacheMisses << ","
            << branchMisses << ","
            << cpuCycles << "\n";
    csvFile.flush();
}