#include "TimingSerializer.hpp"
#include <chrono>
#include "Logger.hpp"
#include "papi.h"
#include <sys/resource.h>
#include <mutex>

std::mutex mtx;

TimingSerializer::TimingSerializer(std::unique_ptr<ISerializer> base, const std::string &csvPath)
    : baseSerializer(std::move(base))
{
    csvFile.open(csvPath, std::ios::app);
    if (!csvFile.tellp())
    {
        csvFile << "timestamp,serializer_type,data_size_bytes,duration_ns,cache_misses,branch_misses,cpu_cycles,max_memory_kb\n";
    }
    if (PAPI_library_init(PAPI_VER_CURRENT) != PAPI_VER_CURRENT)
    {
        LOG_ERROR("PAPI library initialization error!");
        throw std::runtime_error("PAPI library initialization error!");
    }
    if (PAPI_thread_init(pthread_self) != PAPI_OK)
    {
        LOG_ERROR("PAPI thread initialization error!");
        throw std::runtime_error("PAPI thread initialization error!");
    }
}

int TimingSerializer::serialize(const Response &msg)
{
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
        if (PAPI_is_initialized() == PAPI_NOT_INITED)
        {
            LOG_ERROR("PAPI thread level not initialized!");
            return baseSerializer->serialize(msg);
        }

        if (PAPI_create_eventset(&eventSet) != PAPI_OK)
        {
            LOG_ERROR("PAPI create event set error!");
            return baseSerializer->serialize(msg);
        }

        if (PAPI_query_event(PAPI_TOT_CYC) == PAPI_OK)
        {
            if (PAPI_add_event(eventSet, PAPI_TOT_CYC) != PAPI_OK)
            {
                PAPI_perror("PAPI add event PAPI_TOT_CYC error!");
                LOG_ERROR("PAPI add event PAPI_TOT_CYC error!");
                return baseSerializer->serialize(msg);
            }
            cpuCycles = 0;
        }
        else
        {
            LOG_WARN("PAPI event PAPI_TOT_CYC not supported!");
        }

        if (PAPI_query_event(PAPI_L3_TCM) == PAPI_OK)
        {
            if (PAPI_add_event(eventSet, PAPI_L3_TCM) != PAPI_OK)
            {
                PAPI_perror("PAPI add event PAPI_L3_TCM error!");
                LOG_ERROR("PAPI add event PAPI_L3_TCM error!");
                return baseSerializer->serialize(msg);
            }
            cacheMisses = 0;
        }
        else
        {
            LOG_WARN("PAPI event PAPI_L3_TCM not supported!");
        }

        if (PAPI_query_event(PAPI_BR_MSP) == PAPI_OK)
        {
            if (PAPI_add_event(eventSet, PAPI_BR_MSP) != PAPI_OK)
            {
                PAPI_perror("PAPI add event PAPI_BR_MSP error!");
                LOG_ERROR("PAPI add event PAPI_BR_MSP error!");
                return baseSerializer->serialize(msg);
            }
            branchMisses = 0;
        }
        else
        {
            LOG_WARN("PAPI event PAPI_BR_MSP not supported!");
        }

        if (PAPI_start(eventSet) != PAPI_OK)
        {
            LOG_ERROR("PAPI start counters error!");
            PAPI_cleanup_eventset(eventSet);
            PAPI_reset(eventSet);
            PAPI_destroy_eventset(&eventSet);
            PAPI_perror("PAPI start counters error!");
            goto error;
        }
    }

    start = std::chrono::high_resolution_clock::now();
    result = baseSerializer->serialize(msg);
    end = std::chrono::high_resolution_clock::now();
    {

        if (PAPI_stop(eventSet, counters) != PAPI_OK)
        {
            LOG_ERROR("PAPI stop counters error!");
            PAPI_perror("PAPI stop counters error!");
            return result;
        }
        else
        {
            PAPI_cleanup_eventset(eventSet);
            PAPI_reset(eventSet);
            PAPI_destroy_eventset(&eventSet);
        }
    }
    duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    if (cpuCycles == 0)
        cpuCycles = counters[0];
    if (cacheMisses == 0)
        cacheMisses = counters[1];
    if (branchMisses == 0)
        branchMisses = counters[2];

    // Get memory usage
    getrusage(RUSAGE_SELF, &usage);
    maxMemory = usage.ru_maxrss; // in kilobytes

    switch (baseSerializer->getSendMethod())
    {
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
    case SendMethod::PROTOBUF:
        serializerType = "Protobuf";
        break;
    case SendMethod::FlatBuffers:
        serializerType = "FlatBuffers";
        break;
    }
    logTiming(serializerType, result, duration, cacheMisses, branchMisses, cpuCycles, maxMemory);
    return result;

error:
{
    PAPI_stop(eventSet, counters);
    PAPI_cleanup_eventset(eventSet);
    PAPI_reset(eventSet);
    PAPI_destroy_eventset(&eventSet);
}
    return baseSerializer->serialize(msg);
}

void TimingSerializer::logTiming(const std::string &serializerType, size_t dataSize, long long duration, long long cacheMisses, long long branchMisses, long long cpuCycles, long long maxMemory)
{
    std::lock_guard<std::mutex> lock(mtx);
    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
                         now.time_since_epoch())
                         .count();

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