#include "TimingSerializer.hpp"
#include <chrono>
#include <mutex>

std::mutex mtx;

TimingSerializer::TimingSerializer(std::unique_ptr<ISerializer> base, const std::string& csvPath) 
    : baseSerializer(std::move(base)) {
    std::lock_guard<std::mutex> lock(mtx);
    csvFile.open(csvPath, std::ios::app);
    if (!csvFile.tellp()) {
        csvFile << "timestamp,serializer_type,data_size_bytes,duration_ns\n";
    }
}

int TimingSerializer::serialize(const Response& msg) {
    auto start = std::chrono::high_resolution_clock::now();
    int result = baseSerializer->serialize(msg);
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    
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
    logTiming(serializerType, result, duration);
    
    return result;
}

void TimingSerializer::logTiming(const std::string& serializerType, size_t dataSize, long long duration) {
    std::lock_guard<std::mutex> lock(mtx);
    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()).count();
    
    csvFile << timestamp << ","
            << serializerType << ","
            << dataSize << ","
            << duration << "\n";
    csvFile.flush();
}