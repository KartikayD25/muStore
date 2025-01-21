#include "FlatBuffers.hpp"
#include "final_generated.h"
#include <iostream>
#include <sys/socket.h>
FlatBuffers::FlatBuffers() : sendMethod(SendMethod::FlatBuffers) {}
int FlatBuffers::serialize(const Response &msg) {
  flatbuffers::FlatBufferBuilder builder;

  auto a =
      builder.CreateString(msg.fields.empty() ? "" : *msg.fields.at("field1"));
  auto b =
      builder.CreateString(msg.fields.empty() ? "" : *msg.fields.at("field2"));
  auto c =
      builder.CreateString(msg.fields.empty() ? "" : *msg.fields.at("field3"));
  auto d =
      builder.CreateString(msg.fields.empty() ? "" : *msg.fields.at("field4"));
  auto e =
      builder.CreateString(msg.fields.empty() ? "" : *msg.fields.at("field5"));
  auto f =
      builder.CreateString(msg.fields.empty() ? "" : *msg.fields.at("field6"));
  auto g =
      builder.CreateString(msg.fields.empty() ? "" : *msg.fields.at("field7"));
  auto h =
      builder.CreateString(msg.fields.empty() ? "" : *msg.fields.at("field8"));
  auto i =
      builder.CreateString(msg.fields.empty() ? "" : *msg.fields.at("field9"));
  auto j =
      builder.CreateString(msg.fields.empty() ? "" : *msg.fields.at("field0"));
  auto k = builder.CreateString(msg.key + '\n');

  auto person =
      uSer::test::proto::CreatePerson(builder, a, b, c, d, e, f, g, h, i, j, k);
  builder.Finish(person);

  uint8_t *buf = builder.GetBufferPointer();
  int size = builder.GetSize();

  int result = send(msg.clientSocket, buf, size, 0);

  return result;
}
