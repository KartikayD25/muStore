#include "Value.hpp"

void Value::setField(const std::string& field, const std::string& value) {
    if(hasField(field)) {
        delete fields[field];
    }
    fields[field] = new std::string(value);
}

std::string * Value::getField(const std::string& field) const {
    auto it = fields.find(field);
    if(it != fields.end()) {
        return it->second;
    }
    return nullptr;
}

bool Value::hasField(const std::string& field) const {
    return fields.find(field) != fields.end();
}

bool Value::removeField(const std::string& field) {
    // delete the string pointer
    if(hasField(field)) delete fields[field];
    return fields.erase(field) > 0;
}

void Value::setFields(const std::unordered_map<std::string, std::string>& newFields) {
    for(const auto& field : newFields) {
        setField(field.first, field.second);
    }
}

std::unordered_map<std::string, std::string *> Value::getFields() const {
    return fields;
}

std::unordered_map<std::string, std::string *> Value::getSelectedFields(
    const std::vector<std::string>& fieldNames) const {
    std::unordered_map<std::string, std::string *> result;
    for (const auto& field : fieldNames) {
        auto it = fields.find(field);
        if (it != fields.end()) {
            result[field] = it->second;
        }
    }
    return result;
}