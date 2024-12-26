#pragma once
#include "ISerializer.hpp"
class MuSer : public ISerializer {
public:
    MuSer();
    int serialize(const Response& msg) override;
    const SendMethod & getSendMethod()  { return sendMethod; }
private:
    SendMethod sendMethod;
};