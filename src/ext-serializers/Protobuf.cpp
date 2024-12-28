#include <iostream>
#include "Protobuf.hpp"
#include "final.pb.h"
Protobuf::Protobuf() : sendMethod(SendMethod::PROTOBUF) {
}
// if(msg.fields.empty()) {
//         p->set_field1("");
//         p->set_field2("");
//         p->set_field3("");
//         p->set_field4("");
//         p->set_field5("");
//         p->set_field6("");
//         p->set_field7("");
//         p->set_field8("");
//         p->set_field9("");
//         p->set_field10("");
//         p->set_field11(msg.key + '\n');
//         return p->serializeAndWrite(&conn, user::SerializationMethod::ZC);
//     }
int Protobuf::serialize(const Response& msg) {
    Person p;
    if (msg.fields.empty()) {
        p.set_a("");
        p.set_b("");
        p.set_c("");
        p.set_d("");
        p.set_e("");
        p.set_f("");
        p.set_g("");
        p.set_h("");
        p.set_i("");
        p.set_j("");
        p.set_k(msg.key + '\n');
        p.SerializePartialToFileDescriptor(msg.clientSocket);
        return p.ByteSizeLong();
    }
    
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