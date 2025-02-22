#include "TwoCopySerializer.hpp"
#include <sstream>
#include <sys/socket.h>

int TwoCopySerializer::serialize(const Response &msg) {
  std::ostringstream oss;

  switch (msg.type) {
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

  oss << msg.key;
  oss << "\n";
  std::string output = oss.str();
  const char *data = output.c_str();
  size_t totalSent = 0;
  size_t dataSize = output.size();

  while (totalSent < dataSize) {
    ssize_t sent =
        send(msg.clientSocket, data + totalSent, dataSize - totalSent, 0);
    if (sent == -1) {
      return -1; // Handle error
    }
    totalSent += sent;
  }
  return totalSent;
}

void TwoCopySerializer::serializeFields(std::ostringstream &oss,
                                        const Response &msg) {
  oss << msg.fields.size() << " ";
  for (const auto &[field, value] : msg.fields) {
    oss << *value;
  }
}

void TwoCopySerializer::serializeGetFields(std::ostringstream &oss,
                                           const Response &msg) {
  oss << msg.fieldNames.size() << " ";
  for (const auto &field : msg.fieldNames) {
    oss << *msg.fields.at(field);
  }
}

void TwoCopySerializer::serializeGetAll(std::ostringstream &oss,
                                        const Response &msg) {
  oss << msg.fields.size() << " ";
  for (const auto &[field, value] : msg.fields) {
    oss << *value;
  }
}
