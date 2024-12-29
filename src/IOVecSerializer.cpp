#include "IOVecSerializer.hpp"
#include <sys/socket.h>
#include <cstring>

IOVecSerializer::IOVecSerializer() : sendMethod(SendMethod::ONECOPY) {}

IOVecSerializer::IOVecSerializer(SendMethod method) : sendMethod(method) {}


int IOVecSerializer::serialize(const Response& msg) {
    std::vector<iovec> iovecs;
    switch(msg.type) {
        case Response::Type::SET_FIELDS:
        case Response::Type::RESPONSE:
            serializeFields(iovecs,msg);
            break;
        case Response::Type::GET_FIELDS:
            serializeGetFields(iovecs,msg);
            break;
        case Response::Type::GETALL:
            serializeGetAll(iovecs,msg);
            break;
    }

    addIovec(iovecs,msg.key);
    addIovec(iovecs,"\n", 1);

    struct msghdr message = {};
    message.msg_iov = iovecs.data();
    message.msg_iovlen = iovecs.size();
    int flag = sendMethod == SendMethod::ZEROCOPY ? MSG_ZEROCOPY : 0;
    return sendmsg(msg.clientSocket, &message, flag);
}
void IOVecSerializer::addIovec(std::vector<iovec> & iovecs,const char* data, size_t len) {
    iovec iov;
    iov.iov_base = const_cast<char*>(data);
    iov.iov_len = len;
    iovecs.push_back(iov);
}

void IOVecSerializer::addIovec(std::vector<iovec> & iovecs,const std::string& str) {
    addIovec(iovecs,str.c_str(), str.size());
}
void IOVecSerializer::addIovec(std::vector<iovec> & iovecs,const std::string * str) {
    addIovec(iovecs,str->c_str(), str->size());
}

void IOVecSerializer::serializeFields(std::vector<iovec> & iovecs,const Response& msg) {
    for (auto& [field, value] : msg.fields) {
        addIovec(iovecs,value);
    }
}

void IOVecSerializer::serializeGetFields(std::vector<iovec> & iovecs,const Response& msg) {
    for (auto& field : msg.fieldNames) {
        addIovec(iovecs,msg.fields.at(field));
    }
}

void IOVecSerializer::serializeGetAll(std::vector<iovec> & iovecs,const Response& msg) {
    for (auto& [field, value] : msg.fields) {
        addIovec(iovecs,value);
    }
}