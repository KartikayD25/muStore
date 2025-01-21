#include "muSer.hpp"
#include "Logger.hpp"
#include "Person.hpp"
#include "message.hpp"

MuSer::MuSer() : sendMethod(SendMethod::MuSer) {
  user::SymTab::init();
  user::SymTab::readFile(PROTO_FILE);
}

int MuSer::serialize(const Response &msg) {
  using namespace user;
  user::Person p;
  user::net::Conn conn = user::net::Conn(msg.clientSocket);
  if (msg.fields.empty()) {
    p.set_field1("");
    p.set_field2("");
    p.set_field3("");
    p.set_field4("");
    p.set_field5("");
    p.set_field6("");
    p.set_field7("");
    p.set_field8("");
    p.set_field9("");
    p.set_field10("");
    p.set_field11(msg.key + '\n');
  } else {
    p.set_field1(msg.fields.at("field1")->c_str());
    p.set_field2(msg.fields.at("field2")->c_str());
    p.set_field3(msg.fields.at("field3")->c_str());
    p.set_field4(msg.fields.at("field4")->c_str());
    p.set_field5(msg.fields.at("field5")->c_str());
    p.set_field6(msg.fields.at("field6")->c_str());
    p.set_field7(msg.fields.at("field7")->c_str());
    p.set_field8(msg.fields.at("field8")->c_str());
    p.set_field9(msg.fields.at("field9")->c_str());
    p.set_field10(msg.fields.at("field0")->c_str());
    p.set_field11(msg.key + '\n');
  }
#ifdef MU_SER_TC
  LOG_DEBUG("SERIALIZATION METHOD: TWO COPY");
  return p.serializeAndWrite(&conn, user::SerializationMethod::COPY);
#elif MU_SER_OC
  LOG_DEBUG("SERIALIZATION METHOD: ONE COPY");
  return p.serializeAndWrite(&conn, user::SerializationMethod::SG);
#else
  LOG_DEBUG("SERIALIZATION METHOD: ZERO COPY");
  return p.serializeAndWrite(&conn, user::SerializationMethod::ZC);
#endif
}
