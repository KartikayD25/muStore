#include "TwoCopySerializer.hpp"
#include <sstream>
#include <sys/socket.h>

int TwoCopySerializer::serialize(const Response& msg) {
    std::ostringstream oss;
    oss << static_cast<int>(msg.type) << " " 
        << msg.key << " ";

    switch(msg.type) {
        case Response::Type::SET_FIELDS:
        case Response::Type::RESPONSE:
            serializeFields(oss, msg);
            break;
        case Response::Type::GET_FIELDS:
            serializeGetFields(oss, msg);
            break;
        case Response::Type::GETALL:
            serializeGetAll(oss, msg);
            break;
    }

    oss << "\n";
    std::string output = oss.str();
    return send(msg.clientSocket, output.c_str(), output.size(), 0);
}

void TwoCopySerializer::serializeFields(std::ostringstream& oss, const Response& msg) {
    oss << msg.fields.size() << " ";
    for (const auto& [field, value] : msg.fields) {
        oss << field << "=" << *value << " ";
    }
}

void TwoCopySerializer::serializeGetFields(std::ostringstream& oss, const Response& msg) {
    oss << msg.fieldNames.size() << " ";
    for (const auto& field : msg.fieldNames) {
        oss << field << "=" << *msg.fields.at(field) << " ";
    }
}

void TwoCopySerializer::serializeGetAll(std::ostringstream& oss, const Response& msg) {
    oss << msg.fields.size() << " ";
    for (const auto& [field, value] : msg.fields) {
        oss << field << "=" << *value << " ";
    }
}