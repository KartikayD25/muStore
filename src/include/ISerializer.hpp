#pragma once
#include "SendMethod.hpp"
class ISerializer {
public:
    virtual ~ISerializer() = default;
    virtual int serialize(const Response& msg) = 0;
    virtual const SendMethod & getSendMethod() = 0;
};