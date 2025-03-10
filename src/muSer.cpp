#include "muSer.hpp"
#include "Logger.hpp"
#include "message.hpp"
#include "net.hpp"
#include <string>

MuSer::MuSer() : sendMethod(SendMethod::MuSer) {
  user::SymTab::init();
  user::SymTab::readFile(PROTO_FILE);
}

int MuSer::serialize(const Response &msg) {
  using namespace user;
  net::Conn conn(net::ConnProto::TCP, msg.clientSocket);

  std::string msg_name = "TestMessage";
  auto message = user::InitMessage::allocateMessage(msg_name);
  if (msg.fields.empty()) {
    message->addVal("data0", msg.key.c_str());
  } else {
    for (int i = 0; i < msg.fieldNames.size(); i++) {
      message->addVal("data" + std::to_string(i),
                      msg.fields.at(msg.fieldNames.at(i))->c_str());
    }
  }
#ifdef MU_SER_TC
  LOG_DEBUG("SERIALIZATION METHOD: TWO COPY");
  return message->serializeAndWrite(&conn, user::SerializationMethod::COPY);
#elif MU_SER_OC
  LOG_DEBUG("SERIALIZATION METHOD: ONE COPY");
  return message->serializeAndWrite(&conn, user::SerializationMethod::SG);
#else
  LOG_DEBUG("SERIALIZATION METHOD: ZERO COPY");
  return message->serializeAndWrite(&conn, user::SerializationMethod::ZC);
#endif
}
