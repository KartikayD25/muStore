#include "KVStore.hpp"

bool KVStore::insert(const std::string& key) {
    std::lock_guard<std::mutex> lock(mutex);
    if (store.find(key) != store.end()) {
        return false;
    }
    store[key] = Value();
    return true;
}

bool KVStore::setField(const std::string& key, const std::string& field, const std::string& value) {
    std::lock_guard<std::mutex> lock(mutex);
    auto it = store.find(key);
    if (it == store.end()) return false;
    it->second.setField(field, value);
    return true;
}

std::string  * KVStore::getField(const std::string& key, const std::string& field) {
    std::lock_guard<std::mutex> lock(mutex);
    auto it = store.find(key);
    if (it == store.end()) return nullptr;
    return it->second.getField(field);
}

bool KVStore::removeField(const std::string& key, const std::string& field) {
    std::lock_guard<std::mutex> lock(mutex);
    auto it = store.find(key);
    if (it == store.end()) return false;
    return it->second.removeField(field);
}

bool KVStore::remove(const std::string& key) {
    std::lock_guard<std::mutex> lock(mutex);
    return store.erase(key) > 0;
}

bool KVStore::setFields(const std::string& key, 
                       const std::unordered_map<std::string, std::string>& fields) {
    std::lock_guard<std::mutex> lock(mutex);
    auto it = store.find(key);
    if (it == store.end()) return false;
    it->second.setFields(fields);
    return true;
}

std::unordered_map<std::string, std::string *> KVStore::getFields(const std::string& key) {
    std::lock_guard<std::mutex> lock(mutex);
    auto it = store.find(key);
    if (it == store.end()) return {};
    return it->second.getFields();
}

std::unordered_map<std::string, std::string *> KVStore::getSelectedFields(
    const std::string& key, const std::vector<std::string>& fields) {
    std::lock_guard<std::mutex> lock(mutex);
    auto it = store.find(key);
    if (it == store.end()) return {};
    return it->second.getSelectedFields(fields);
}