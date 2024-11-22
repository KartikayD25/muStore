#include "Protocol.hpp"
#include <sstream>
#include <sys/socket.h>
#include <cstring>
int Protocol::serializeAndSend(Message& msg) {
    if(msg.sendMethod == Message::SendMethod::TWOCOPY) {
        std::ostringstream oss;
    oss << static_cast<int>(msg.type) << FIELD_SEP
        << msg.key << FIELD_SEP;

    if (msg.type == Message::Type::SET_FIELDS || 
        msg.type == Message::Type::RESPONSE) {
        oss << msg.fields.size() << FIELD_SEP;
        for (const auto& field : msg.fields) {
            oss << field.first << FIELD_SEP;
    }
        }
    else if (msg.type == Message::Type::GET_FIELDS) {
        oss << msg.fieldNames.size() << FIELD_SEP;
        for (auto field : msg.fieldNames) {
            oss << field << KV_SEP << *(msg.fields[field]) << FIELD_SEP;
        }
    }
    else if (msg.type == Message::Type::GETALL) {
        oss << msg.fields.size() << FIELD_SEP;
        for (const auto& field : msg.fields) {
            oss << field.first << KV_SEP << *(field.second) << FIELD_SEP;
        }
    }
        int result = send(msg.clientSocket, oss.str().c_str(), oss.str().size(), 0);
        return result;
    }
    else if(msg.sendMethod == Message::SendMethod::ONECOPY) {
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

        if (msg.type == Message::Type::SET_FIELDS || 
            msg.type == Message::Type::RESPONSE) {

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
        else if (msg.type == Message::Type::GET_FIELDS) {
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
        else if (msg.type == Message::Type::GETALL) {
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
    else if(msg.sendMethod == Message::SendMethod::ONECOPY) {
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

        if (msg.type == Message::Type::SET_FIELDS || 
            msg.type == Message::Type::RESPONSE) {

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
        else if (msg.type == Message::Type::GET_FIELDS) {
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
        else if (msg.type == Message::Type::GETALL) {
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
}

Message Protocol::deserialize(const std::string& data) {
    Message msg;
    std::istringstream iss(data);
    std::string token;
    int type;

    std::getline(iss, token, FIELD_SEP);
    type = std::stoi(token);
    msg.type = static_cast<Message::Type>(type);

    std::getline(iss, msg.key, FIELD_SEP);

    if (msg.type == Message::Type::SET_FIELDS || 
        msg.type == Message::Type::RESPONSE) {
        std::getline(iss, token, FIELD_SEP);
        int fieldCount = std::stoi(token);
        
        for (int i = 0; i < fieldCount; i++) {
            std::string field, value;
            std::getline(iss, field, KV_SEP);
            std::getline(iss, value, FIELD_SEP);
            msg.fields[field] = value;
        }
    }
    else if (msg.type == Message::Type::GET_SELECTED_FIELDS) {
        std::getline(iss, token, FIELD_SEP);
        int fieldCount = std::stoi(token);
        
        for (int i = 0; i < fieldCount; i++) {
            std::string field;
            std::getline(iss, field, FIELD_SEP);
            msg.fieldNames.push_back(field);
        }
    }

    return msg;
}