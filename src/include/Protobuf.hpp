#pragma once
#include "ISerializer.hpp"
class Protobuf : public ISerializer {
public:
    Protobuf();
    int serialize(const Response& msg) override;
    const SendMethod & getSendMethod()  { return sendMethod; }
private:
    SendMethod sendMethod;
};