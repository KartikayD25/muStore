#include "Protocol.hpp"
#include <sstream>
#include <sys/socket.h>
#include <cstring>
#include "Logger.hpp"
const char Protocol::FIELD_SEP = ' ';
const char Protocol::KV_SEP = '=';
int Protocol::serializeAndSend(Response& msg) {
    LOG_INFO("Serializing and sending message " + std::to_string(static_cast<int>(msg.type)));
    if(msg.sendMethod == Response::SendMethod::TWOCOPY) {
        LOG_INFO("Using two-copy method");
        std::ostringstream oss;
    oss << static_cast<int>(msg.type) << FIELD_SEP
        << msg.key << FIELD_SEP;

    if (msg.type == Response::Type::SET_FIELDS || 
        msg.type == Response::Type::RESPONSE) {
        oss << msg.fields.size() << FIELD_SEP;
        for (const auto& field : msg.fields) {
            oss << field.first << FIELD_SEP;
    }
        }
    else if (msg.type == Response::Type::GET_FIELDS) {
        oss << msg.fieldNames.size() << FIELD_SEP;
        for (auto field : msg.fieldNames) {
            oss << field << KV_SEP << *(msg.fields[field]) << FIELD_SEP;
        }
    }
    else if (msg.type == Response::Type::GETALL) {
        oss << msg.fields.size() << FIELD_SEP;
        for (const auto& field : msg.fields) {
            oss << field.first << KV_SEP << *(field.second) << FIELD_SEP;
        }
    }
        LOG_INFO("Sending message: " + oss.str());
        int result = send(msg.clientSocket, oss.str().c_str(), oss.str().size(), 0);
        if (result == -1) {
            LOG_ERROR("Error sending message");
        }
        return result;
    }
    else if(msg.sendMethod == Response::SendMethod::ONECOPY) {
        LOG_INFO("Using one-copy method");
        std::vector<iovec> iovecs;
        iovecs.reserve(100);
        iovec iovType;
        iovType.iov_base = const_cast<char*>(std::to_string(static_cast<int>(msg.type)).c_str());
        iovType.iov_len = std::to_string(static_cast<int>(msg.type)).size();
        iovecs.push_back(iovType);
        iovec iovFieldSep;
        iovFieldSep.iov_base = const_cast<char*>(&FIELD_SEP);
        iovFieldSep.iov_len = 1;
        iovecs.push_back(iovFieldSep);
        iovec iovKey;
        iovKey.iov_base = const_cast<char*>(msg.key.c_str());
        iovKey.iov_len = msg.key.size();
        iovecs.push_back(iovKey);
        iovecs.push_back(iovFieldSep);
        iovec iovKVSep;
        iovKVSep.iov_base = const_cast<char*>(&KV_SEP);
        iovKVSep.iov_len = 1;

        if (msg.type == Response::Type::SET_FIELDS || 
            msg.type == Response::Type::RESPONSE) {

                iovec iovFieldCount;
                iovFieldCount.iov_base = const_cast<char*>(std::to_string(msg.fields.size()).c_str());
                iovFieldCount.iov_len = std::to_string(msg.fields.size()).size();
                iovecs.push_back(iovFieldCount);
                iovecs.push_back(iovFieldSep);
            for (const auto& field : msg.fields) {
                iovec iovFieldName;
                iovFieldName.iov_base = const_cast<char*>(field.first.c_str());
                iovFieldName.iov_len = field.first.size();
                iovecs.push_back(iovFieldName);
                iovecs.push_back(iovFieldSep);
            }
        }
        else if (msg.type == Response::Type::GET_FIELDS) {
            iovec iovFieldCount;
            iovFieldCount.iov_base = const_cast<char*>(std::to_string(msg.fieldNames.size()).c_str());
            iovFieldCount.iov_len = std::to_string(msg.fieldNames.size()).size();
            iovecs.push_back(iovFieldCount);
            for (const auto& field : msg.fieldNames) {
                iovec iovFieldName;
                iovFieldName.iov_base = const_cast<char*>(field.c_str());
                iovFieldName.iov_len = field.size();
                iovecs.push_back(iovFieldName);
                iovecs.push_back(iovKVSep);
                iovec iovdata;
                iovdata.iov_base = const_cast<char*>(msg.fields[field]->c_str());
                iovdata.iov_len = field.size();
                iovecs.push_back(iovdata);
                iovecs.push_back(iovFieldSep);
            }
        }
        else if (msg.type == Response::Type::GETALL) {
            iovec iovFieldCount;
            iovFieldCount.iov_base = const_cast<char*>(std::to_string(msg.fields.size()).c_str());
            iovFieldCount.iov_len = std::to_string(msg.fields.size()).size();
            iovecs.push_back(iovFieldCount);
            for (const auto& field : msg.fields) {
                iovec iovFieldName;
                iovFieldName.iov_base = const_cast<char*>(field.first.c_str());
                iovFieldName.iov_len = field.first.size();
                iovecs.push_back(iovKVSep);
                iovec iovdata;
                iovdata.iov_base = const_cast<char*>(field.second->c_str());
                iovdata.iov_len = field.second->size();
                iovecs.push_back(iovdata);
                iovecs.push_back(iovFieldSep);
            }        
        }
        struct msghdr message;
        memset(&message, 0, sizeof(message));
        message.msg_iov = iovecs.data();
        message.msg_iovlen = iovecs.size();
        int result = sendmsg(msg.clientSocket, &message, 0);
        return result;
    }
    else if(msg.sendMethod == Response::SendMethod::ZEROCOPY) {
        LOG_INFO("Using zero-copy method");
        std::vector<iovec> iovecs;
        iovecs.reserve(100);
        iovec iovType;
        iovType.iov_base = const_cast<char*>(std::to_string(static_cast<int>(msg.type)).c_str());
        iovType.iov_len = std::to_string(static_cast<int>(msg.type)).size();
        iovecs.push_back(iovType);
        iovec iovFieldSep;
        iovFieldSep.iov_base = const_cast<char*>(&FIELD_SEP);
        iovFieldSep.iov_len = 1;
        iovecs.push_back(iovFieldSep);
        iovec iovKey;
        iovKey.iov_base = const_cast<char*>(msg.key.c_str());
        iovKey.iov_len = msg.key.size();
        iovecs.push_back(iovKey);
        iovecs.push_back(iovFieldSep);
        iovec iovKVSep;
        iovKVSep.iov_base = const_cast<char*>(&KV_SEP);
        iovKVSep.iov_len = 1;

        if (msg.type == Response::Type::SET_FIELDS || 
            msg.type == Response::Type::RESPONSE) {

                iovec iovFieldCount;
                iovFieldCount.iov_base = const_cast<char*>(std::to_string(msg.fields.size()).c_str());
                iovFieldCount.iov_len = std::to_string(msg.fields.size()).size();
                iovecs.push_back(iovFieldCount);
                iovecs.push_back(iovFieldSep);
            for (const auto& field : msg.fields) {
                iovec iovFieldName;
                iovFieldName.iov_base = const_cast<char*>(field.first.c_str());
                iovFieldName.iov_len = field.first.size();
                iovecs.push_back(iovFieldName);
                iovecs.push_back(iovFieldSep);
            }
        }
        else if (msg.type == Response::Type::GET_FIELDS) {
            iovec iovFieldCount;
            iovFieldCount.iov_base = const_cast<char*>(std::to_string(msg.fieldNames.size()).c_str());
            iovFieldCount.iov_len = std::to_string(msg.fieldNames.size()).size();
            iovecs.push_back(iovFieldCount);
            for (const auto& field : msg.fieldNames) {
                iovec iovFieldName;
                iovFieldName.iov_base = const_cast<char*>(field.c_str());
                iovFieldName.iov_len = field.size();
                iovecs.push_back(iovFieldName);
                iovecs.push_back(iovKVSep);
                iovec iovdata;
                iovdata.iov_base = const_cast<char*>(msg.fields[field]->c_str());
                iovdata.iov_len = field.size();
                iovecs.push_back(iovdata);
                iovecs.push_back(iovFieldSep);
            }
        }
        else if (msg.type == Response::Type::GETALL) {
            iovec iovFieldCount;
            iovFieldCount.iov_base = const_cast<char*>(std::to_string(msg.fields.size()).c_str());
            iovFieldCount.iov_len = std::to_string(msg.fields.size()).size();
            iovecs.push_back(iovFieldCount);
            for (const auto& field : msg.fields) {
                iovec iovFieldName;
                iovFieldName.iov_base = const_cast<char*>(field.first.c_str());
                iovFieldName.iov_len = field.first.size();
                iovecs.push_back(iovKVSep);
                iovec iovdata;
                iovdata.iov_base = const_cast<char*>(field.second->c_str());
                iovdata.iov_len = field.second->size();
                iovecs.push_back(iovdata);
                iovecs.push_back(iovFieldSep);
            }        
        }
        struct msghdr message;
        memset(&message, 0, sizeof(message));
        message.msg_iov = iovecs.data();
        message.msg_iovlen = iovecs.size();
        int result = sendmsg(msg.clientSocket, &message, 0);
        return result;
    }
    return -1;
}

Request Protocol::deserialize(const std::string& data) {
    Request msg;
    std::istringstream iss(data);
    std::string command;    
    iss >> command;
    if (command == "SET_FIELDS") {
        msg.type = Request::Type::SET_FIELDS;
        iss >> msg.key;        
        std::string pair;
        while (iss >> pair) {
            size_t pos = pair.find(KV_SEP);
            if (pos == std::string::npos) continue;
            
            std::string field = pair.substr(0, pos);
            // Remove quotes if present
            std::string value = pair.substr(pos + 1);
            if (value.front() == '"' && value.back() == '"') {
                value = value.substr(1, value.length() - 2);
            }
            
            msg.fields[field] = value;
            msg.fieldNames.push_back(field);
        }
    }
    else if (command == "GETALL") {
        msg.type = Request::Type::GETALL;
        iss >> msg.key;
    }
    else if (command == "GET_FIELDS") {
        msg.type = Request::Type::GET_FIELDS;
        iss >> msg.key;        
        std::string field;
        while (iss >> field) {
            msg.fieldNames.push_back(field);
        }
    }
    else {
        msg.type = Request::Type::INVALID;
    }
    
    return msg;
}