#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include "ISerializer.hpp"
#include <memory>
class Protocol {
public:
    static int serializeAndSend(Response& msg);
    static Request deserialize(const std::string& data);
    static int createSocket(int port);
private:
    static std::unique_ptr<ISerializer> serializer;
    static const char FIELD_SEP;
    static const char KV_SEP;
};