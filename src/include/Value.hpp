#pragma once
#include <string>
#include <unordered_map>
#include <vector>

class Value {
public:
    void setField(const std::string& field, const std::string& value);
    std::string * getField(const std::string& field) const;
    bool hasField(const std::string& field) const;
    bool removeField(const std::string& field);
    void setFields(const std::unordered_map<std::string, std::string>& newFields);
    std::unordered_map<std::string, std::string *> getFields() const;
    std::unordered_map<std::string, std::string *> getSelectedFields(const std::vector<std::string>& fields) const;
    
private:
    std::unordered_map<std::string, std::string *> fields;
};