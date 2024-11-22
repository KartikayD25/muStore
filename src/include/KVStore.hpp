#pragma once
#include "Value.hpp"
#include <vector>
#include <mutex>
#include <unordered_map>

class KVStore {
public:
    bool insert(const std::string& key);
    bool setField(const std::string& key, const std::string& field, const std::string& value);
    std::string * getField(const std::string& key, const std::string& field);
    bool removeField(const std::string& key, const std::string& field);
    bool remove(const std::string& key);
    bool setFields(const std::string& key, 
                  const std::unordered_map<std::string, std::string>& fields);
    std::unordered_map<std::string, std::string *> getFields(const std::string& key);
    std::unordered_map<std::string, std::string *> getSelectedFields(
        const std::string& key, const std::vector<std::string>& fields);

private:
    std::unordered_map<std::string, Value> store;
    std::mutex mutex;
};