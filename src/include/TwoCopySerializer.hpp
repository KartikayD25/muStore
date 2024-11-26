#pragma once
#include "ISerializer.hpp"
#include "Protocol.hpp"
#include "SendMethod.hpp"
class TwoCopySerializer : public ISerializer {
public:
    int serialize(const Response& msg) override;
    const SendMethod & getSendMethod() override { return sendMethod; }
private:
    SendMethod sendMethod = SendMethod::TWOCOPY;
    void serializeFields(std::ostringstream& oss, const Response& msg);
    void serializeGetFields(std::ostringstream& oss, const Response& msg);
    void serializeGetAll(std::ostringstream& oss, const Response& msg);
};