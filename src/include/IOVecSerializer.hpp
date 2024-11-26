#pragma once
#include "ISerializer.hpp"
#include "Protocol.hpp"
#include <vector>
#include <sys/uio.h>

class IOVecSerializer : public ISerializer {
public:
    int serialize(const Response& msg) override;
    IOVecSerializer();
    IOVecSerializer(SendMethod method);
    const SendMethod & getSendMethod()  { return sendMethod; }
private:
    std::vector<iovec> iovecs;
    SendMethod sendMethod;
    void addIovec(const char* data, size_t len);
    void addIovec(const std::string & string);
    void serializeFields(const Response& msg);
    void serializeGetFields(const Response& msg);
    void serializeGetAll(const Response& msg);
};