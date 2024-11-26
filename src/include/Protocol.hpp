#pragma once
#include <string>
#include <vector>
#include <unordered_map>
struct Response {
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

struct Request {
    enum class Type {
        SET_FIELDS,
        GETALL,
        GET_FIELDS,
        INVALID
    };
    Type type;
    std::string key;
    std::unordered_map<std::string, std::string> fields;
    std::vector<std::string> fieldNames;  // for GET_SELECTED_FIELDS
    int clientSocket;
};

class Protocol {
public:
    static int serializeAndSend(Response& msg);
    static Request deserialize(const std::string& data);
private:
    static const char FIELD_SEP;
    static const char KV_SEP;
};