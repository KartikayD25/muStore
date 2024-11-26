#include "TimingSerializer.hpp"
#include <chrono>

TimingSerializer::TimingSerializer(std::unique_ptr<ISerializer> base, const std::string& csvPath) 
    : baseSerializer(std::move(base)) {
    csvFile.open(csvPath, std::ios::app);
    if (!csvFile.tellp()) {
        csvFile << "timestamp,serializer_type,data_size_bytes,duration_ns\n";
    }
}

int TimingSerializer::serialize(const Response& msg) {
    size_t dataSize = msg.key.size();
    for (const auto& [field, value] : msg.fields) {
        dataSize += field.size() + value->size();
    }

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
    }
    logTiming(serializerType, dataSize, duration);
    
    return result;
}

void TimingSerializer::logTiming(const std::string& serializerType, size_t dataSize, long long duration) {
    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()).count();
    
    csvFile << timestamp << ","
            << serializerType << ","
            << dataSize << ","
            << duration << "\n";
    csvFile.flush();
}