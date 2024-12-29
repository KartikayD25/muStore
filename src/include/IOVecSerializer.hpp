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
    SendMethod sendMethod;
    void addIovec(std::vector<iovec> & iovecs, const char* data, size_t len);
    void addIovec(std::vector<iovec> & iovecs, const std::string & string);
    void addIovec(std::vector<iovec> & iovecs,const std::string * string);
    void serializeFields(std::vector<iovec> & iovecs,const Response& msg);
    void serializeGetFields(std::vector<iovec> & iovecs,const Response& msg);
    void serializeGetAll(std::vector<iovec> & iovecs,const Response& msg);
};