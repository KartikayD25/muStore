#include <iostream>
#include "Protobuf.hpp"
#include "final.pb.h"
Protobuf::Protobuf() : sendMethod(SendMethod::PROTOBUF) {
}

int Protobuf::serialize(const Response& msg) {
    Person p;
    p.set_a(*msg.fields.at("field1"));
    p.set_b(*msg.fields.at("field2"));
    p.set_c(*msg.fields.at("field3"));
    p.set_d(*msg.fields.at("field4"));
    p.set_e(*msg.fields.at("field5"));
    p.set_f(*msg.fields.at("field6"));
    p.set_g(*msg.fields.at("field7"));
    p.set_h(*msg.fields.at("field8"));
    p.set_i(*msg.fields.at("field9"));
    p.set_j(*msg.fields.at("field0"));
    p.set_k(msg.key);
    p.SerializePartialToFileDescriptor(msg.clientSocket);
    return p.ByteSizeLong();
}