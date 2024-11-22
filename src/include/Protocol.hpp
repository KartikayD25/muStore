// Protocol.hpp - message format
#pragma once
#include <string>
#include <vector>
#include <unordered_map>
struct Message {
    enum class Type {
        SET_FIELDS,
        GETALL,
        GET_FIELDS,
        RESPONSE
    };
    enum class SendMethod {
    TWOCOPY,
    ONECOPY,
    ZEROCOPY,
    MuSer
    };

    Type type;
    SendMethod sendMethod;
    std::string key;
    std::unordered_map<std::string, std::string*> fields;
    std::vector<std::string> fieldNames;  // for GET_SELECTED_FIELDS
    int clientSocket;
};

class Protocol {
public:
    static int serializeAndSend(Message& msg);
    static Message deserialize(const std::string& data);
private:
    static const char FIELD_SEP = '\t';
    static const char KV_SEP = '=';
};