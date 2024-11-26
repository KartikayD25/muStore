#pragma once
#include "ISerializer.hpp"
#include <chrono>
#include <fstream>
#include <memory>
class TimingSerializer : public ISerializer {
public:
    TimingSerializer(std::unique_ptr<ISerializer> base, const std::string& csvPath);
    int serialize(const Response& msg) override;
    const SendMethod & getSendMethod() override { return baseSerializer->getSendMethod(); }
private:
    std::unique_ptr<ISerializer> baseSerializer;
    std::ofstream csvFile;
    void logTiming(const std::string& serializerType, size_t dataSize, long long duration);
};