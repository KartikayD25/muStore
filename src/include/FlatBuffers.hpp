#pragma once
#include "ISerializer.hpp"
class FlatBuffers : public ISerializer {
public:
    FlatBuffers();
    int serialize(const Response& msg) override;
    const SendMethod & getSendMethod()  { return sendMethod; }
private:
    SendMethod sendMethod;
};