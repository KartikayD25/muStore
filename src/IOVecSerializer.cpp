#include "IOVecSerializer.hpp"
#include <sys/socket.h>
#include <cstring>

IOVecSerializer::IOVecSerializer() : sendMethod(SendMethod::ONECOPY) {}

IOVecSerializer::IOVecSerializer(SendMethod method) : sendMethod(method) {}


int IOVecSerializer::serialize(const Response& msg) {
    iovecs.clear();
    std::string type = std::to_string(static_cast<int>(msg.type));
    addIovec(type);
    addIovec(" ", 1);
    addIovec(msg.key);
    addIovec(" ", 1);

    switch(msg.type) {
        case Response::Type::SET_FIELDS:
        case Response::Type::RESPONSE:
            serializeFields(msg);
            break;
        case Response::Type::GET_FIELDS:
            serializeGetFields(msg);
            break;
        case Response::Type::GETALL:
            serializeGetAll(msg);
            break;
    }

    addIovec("\n", 1);

    struct msghdr message = {};
    message.msg_iov = iovecs.data();
    message.msg_iovlen = iovecs.size();
    int flag = sendMethod == SendMethod::ZEROCOPY ? MSG_ZEROCOPY : 0;
    return sendmsg(msg.clientSocket, &message, flag);
}
void IOVecSerializer::addIovec(const char* data, size_t len) {
    iovec iov;
    iov.iov_base = const_cast<char*>(data);
    iov.iov_len = len;
    iovecs.push_back(iov);
}

void IOVecSerializer::addIovec(const std::string& str) {
    addIovec(str.c_str(), str.size());
}

void IOVecSerializer::serializeFields(const Response& msg) {
    std::string count = std::to_string(msg.fields.size());
    addIovec(count);
    addIovec(" ", 1);
    
    for (auto& [field, value] : msg.fields) {
        addIovec(field);
        addIovec("=", 1);
        addIovec(*value);
        addIovec(" ", 1);
    }
}

void IOVecSerializer::serializeGetFields(const Response& msg) {
    std::string count = std::to_string(msg.fieldNames.size());
    addIovec(count);
    addIovec(" ", 1);
    
    for (auto& field : msg.fieldNames) {
        addIovec(field);
        addIovec("=", 1);
        addIovec(*msg.fields.at(field));
        addIovec(" ", 1);
    }
}

void IOVecSerializer::serializeGetAll(const Response& msg) {
    std::string count = std::to_string(msg.fields.size());
    addIovec(count);
    addIovec(" ", 1);
    
    for (auto& [field, value] : msg.fields) {
        addIovec(field);
        addIovec("=", 1);
        addIovec(*value);
        addIovec(" ", 1);
    }
}