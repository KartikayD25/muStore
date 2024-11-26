#pragma once
#include <string>
#include <vector>
#include <unordered_map>
enum class SendMethod {
    TWOCOPY,
    ONECOPY,
    ZEROCOPY,
    MuSer
    };
struct Response {
    enum class Type {
        SET_FIELDS,
        GETALL,
        GET_FIELDS,
        RESPONSE
    };
    
    Type type;
    std::string key;
    std::unordered_map<std::string, std::string*> fields;
    std::vector<std::string> fieldNames;  // for GET_SELECTED_FIELDS
    int clientSocket;
};
typedef struct Response Response;

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