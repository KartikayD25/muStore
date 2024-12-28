#include "Protocol.hpp"
#include "Protobuf.hpp"
#include "TwoCopySerializer.hpp"
#include "IOVecSerializer.hpp"
#include "muSer.hpp"
#include <sstream>
#include <unistd.h>
#include <sys/socket.h>
#include <cstring>
#include "Logger.hpp"
#include <chrono>
#include "FlatBuffers.hpp"
#include "TimingSerializer.hpp"

const char Protocol::FIELD_SEP = ' ';
const char Protocol::KV_SEP = '=';

#ifdef ONE_COPY
std::unique_ptr<ISerializer> Protocol::serializer = std::make_unique<TimingSerializer>(
    std::make_unique<IOVecSerializer>(),
    "serializer_timing.csv"
);
#endif

#ifdef MU_SER
std::unique_ptr<ISerializer> Protocol::serializer = std::make_unique<TimingSerializer>(
    std::make_unique<MuSer>(),
    "serializer_timing.csv"
);
#endif

#ifdef ZERO_COPY
std::unique_ptr<ISerializer> Protocol::serializer = std::make_unique<TimingSerializer>(
    std::make_unique<IOVecSerializer>(SendMethod::ZEROCOPY),
    "serializer_timing.csv"
);
#endif

#ifdef TWO_COPY
std::unique_ptr<ISerializer> Protocol::serializer = std::make_unique<TimingSerializer>(
    std::make_unique<TwoCopySerializer>(),
    "serializer_timing.csv"
); 
#endif

#ifdef PROTO_BUF
std::unique_ptr<ISerializer> Protocol::serializer = std::make_unique<TimingSerializer>(
    std::make_unique<Protobuf>(),
    "serializer_timing.csv"
); 
#endif

#ifdef FLAT_BUF
std::unique_ptr<ISerializer> Protocol::serializer = std::make_unique<TimingSerializer>(
    std::make_unique<FlatBuffers>(),
    "serializer_timing.csv"
); 
#endif


int Protocol::serializeAndSend(Response& msg) {
    return serializer->serialize(msg);  
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


int Protocol::createSocket(int port) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        throw std::runtime_error("Failed to create socket");
    }
    int opt = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        close(sock);
        throw std::runtime_error("Failed to set SO_REUSEADDR");
    }

    if (setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, &opt, sizeof(opt)) < 0) {
        close(sock);
        throw std::runtime_error("Failed to set SO_KEEPALIVE");
    }
    // if Protocol::SendMethod is ZEROCOPY, set SO_ZEROCOPY
    if (serializer->getSendMethod() == SendMethod::ZEROCOPY) {
        if (setsockopt(sock, SOL_SOCKET, SO_ZEROCOPY, &opt, sizeof(opt)) < 0) {
            close(sock);
            throw std::runtime_error("Failed to set SO_ZEROCOPY");
        }
    }
    return sock;
}