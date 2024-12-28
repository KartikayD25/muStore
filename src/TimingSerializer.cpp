#include "TimingSerializer.hpp"
#include <chrono>
#include <mutex>
#include "papi.h"
#include <sys/resource.h>

std::mutex mtx;

TimingSerializer::TimingSerializer(std::unique_ptr<ISerializer> base, const std::string& csvPath) 
    : baseSerializer(std::move(base)) {
    std::lock_guard<std::mutex> lock(mtx);
    csvFile.open(csvPath, std::ios::app);
    if (!csvFile.tellp()) {
        csvFile << "timestamp,serializer_type,data_size_bytes,duration_ns,cache_misses,branch_misses,total_cpu_cycles,max_memory_kb\n";
    }
}

int TimingSerializer::serialize(const Response& msg) {
    // Initialize PAPI
    if (PAPI_library_init(PAPI_VER_CURRENT) != PAPI_VER_CURRENT) {
        throw std::runtime_error("PAPI library initialization error");
    }

    // Define the events to monitor
    int events[3] = {PAPI_L3_TCM, PAPI_BR_MSP, PAPI_TOT_CYC};
    long long values[3] = {-1, -1, -1}; // Default to -1 for unsupported events

    // Start counting
    if (PAPI_start_counters(events, 3) != PAPI_OK) {
        return baseSerializer->serialize(msg);
    }

    auto start = std::chrono::high_resolution_clock::now();
    int result = baseSerializer->serialize(msg);
    auto end = std::chrono::high_resolution_clock::now();

    // Stop counting and read the values
    if (PAPI_stop_counters(values, 3) != PAPI_OK) {
        return result;
    }

    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    // Get memory usage
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    long maxMemoryKB = usage.ru_maxrss; // Maximum resident set size in kilobytes

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
        case SendMethod::FlatBuffers:
            serializerType = "FlatBuffers";
            break;
        case SendMethod::PROTOBUF:
            serializerType = "ProtoBuf";
            break;
    }
    logTiming(serializerType, result, duration, values[0], values[1], values[2], maxMemoryKB);

    return result;
}

void TimingSerializer::logTiming(const std::string& serializerType, size_t dataSize, long long duration, long long cacheMisses, long long branchMisses, long long totalCycles, long maxMemoryKB) {
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
            << totalCycles << ","
            << maxMemoryKB << "\n";
    csvFile.flush();
}