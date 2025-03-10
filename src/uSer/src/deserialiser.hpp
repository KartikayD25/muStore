#pragma once

#include <exception>
#include <optional>

#include "define.hpp"
#include "deserialise_message_meta.hpp"
#include "symtab.hpp"

namespace user {

class Deserialiser {
public:
  Deserialiser(std::string message_name, char *buf, size_t buf_len)
      : message_name_(message_name), symbol_(SYMTAB->getSymbol(message_name_)),
        buf_(buf), buf_len_(buf_len) {
    init();
  }

  /*
   * Get the value
   * Only for primitive type(simple), non-repeated
   * Type safe
   */
  template <typename T>
  auto get(const std::string &mem_name) -> std::optional<T> {
    uint32_t id = symbol_->get_member_id(mem_name);
    assert(id <= symbol_->members_.back().id_);
    auto mem_sym = symbol_->members_[id - 1];
    assert(!TypeHelper::isComplexType(symbol_->members_[id - 1].type_) &&
           !mem_sym.repeated_);

    if (!isPresent(id)) {
      return std::nullopt;
    }
    if (!TypeHelper::isSamePrimitveType<T>(mem_sym.type_)) {
      throw std::bad_typeid();
    }

    auto it = desrial_item_map_.find(id);
    if (it == desrial_item_map_.end()) {
      size_t offset = calcOffset(id);
      assert(offset + 1 <= buf_len_);
      T val = *reinterpret_cast<const T *>(
          reinterpret_cast<gen_ptr_t>(buf_ + offset));
      desrial_item_map_.insert({id, new PrimitiveSingleMeta<T>(val)});
      return val;
    }
    return dynamic_cast<PrimitiveSingleMeta<T> *>(it->second)->get();
  }

  /*
   * Get the value
   * Only for primitive type(simple), repeated
   * Type safe
   */
  template <typename T>
  auto get(const std::string &mem_name, uint32_t idx) -> std::optional<T> {
    uint32_t id = symbol_->get_member_id(mem_name);
    assert(id <= symbol_->members_.back().id_);
    auto mem_sym = symbol_->members_[id - 1];
    assert(!TypeHelper::isComplexType(symbol_->members_[id - 1].type_) &&
           mem_sym.repeated_);

    if (!isPresent(id)) {
      return std::nullopt;
    }
    if (!TypeHelper::isSamePrimitveType<T>(mem_sym.type_)) {
      throw std::bad_typeid();
    }
    auto it = desrial_item_map_.find(id);
    if (it == desrial_item_map_.end()) {
      // offset of heaer<listOffset, size>
      auto offset = calcOffset(id);
      auto hdr_base = buf_ + offset;
      // assert(offset + 1 <= buf_len_);
      decodeAndStorePrimitiveList(hdr_base, mem_sym);
      it = desrial_item_map_.find(id);
    }
    return dynamic_cast<PrimitiveRepeatedMeta *>(it->second)->get<T>(idx);
  }

  auto getByte(const std::string &mem_name) -> std::optional<byte_t> {
    uint32_t id = symbol_->get_member_id(mem_name);
    assert(id <= symbol_->members_.back().id_);
    auto mem_sym = symbol_->members_[id - 1];
    assert(TypeHelper::isComplexType(symbol_->members_[id - 1].type_) &&
           !mem_sym.repeated_);

    if (!isPresent(id)) {
      return std::nullopt;
    }
    auto it = desrial_item_map_.find(id);
    if (it == desrial_item_map_.end()) {
      // offset of heaer<len, bytes....>
      auto offset = calcOffset(id);
      auto bytes_base = buf_ + offset + 4;
      uint32_t len = *reinterpret_cast<const uint32_t *>(buf_ + offset);

      // assert(offset + 1 <= buf_len_);
      desrial_item_map_.insert({id, new ByteSingleMeta(bytes_base, len)});
      it = desrial_item_map_.find(id);
    }
    return dynamic_cast<ByteSingleMeta *>(it->second)->get();
  }
  auto getString(const std::string &mem_name) -> std::optional<std::string> {
    uint32_t id = symbol_->get_member_id(mem_name);
    assert(id <= symbol_->members_.back().id_);
    auto mem_sym = symbol_->members_[id - 1];
    assert(TypeHelper::isComplexType(symbol_->members_[id - 1].type_) &&
           !mem_sym.repeated_);

    if (!isPresent(id)) {
      return std::nullopt;
    }
    auto it = desrial_item_map_.find(id);
    if (it == desrial_item_map_.end()) {
      // offset of heaer<len, bytes....>
      auto offset = calcOffset(id);
      auto bytes_base = buf_ + offset + 4;
      uint32_t len = *reinterpret_cast<const uint32_t *>(buf_ + offset);

      // assert(offset + 1 <= buf_len_);
      desrial_item_map_.insert({id, new ByteSingleMeta(bytes_base, len)});
      it = desrial_item_map_.find(id);
    }
    return dynamic_cast<ByteSingleMeta *>(it->second)->getString();
  }

  auto assignString(const std::string &mem_name, std::string &str) -> void {
    uint32_t id = symbol_->get_member_id(mem_name);
    assert(id <= symbol_->members_.back().id_);
    auto mem_sym = symbol_->members_[id - 1];
    assert(TypeHelper::isComplexType(symbol_->members_[id - 1].type_) &&
           !mem_sym.repeated_);

    if (!isPresent(id)) {
      return;
    }
    auto it = desrial_item_map_.find(id);
    if (it == desrial_item_map_.end()) {
      // offset of heaer<len, bytes....>
      auto offset = calcOffset(id);
      auto bytes_base = buf_ + offset + 4;
      uint32_t len = *reinterpret_cast<const uint32_t *>(buf_ + offset);

      // assert(offset + 1 <= buf_len_);
      desrial_item_map_.insert({id, new ByteSingleMeta(bytes_base, len)});
      it = desrial_item_map_.find(id);
    }
    dynamic_cast<ByteSingleMeta *>(it->second)->assignString(str);
  }

  /*
   * Bytes
   * Repeated
   */
  auto getByte(const std::string &mem_name, uint32_t idx)
      -> std::optional<byte_t> {
    uint32_t id = symbol_->get_member_id(mem_name);
    assert(id <= symbol_->members_.back().id_);
    auto mem_sym = symbol_->members_[id - 1];
    assert(TypeHelper::isComplexType(symbol_->members_[id - 1].type_) &&
           mem_sym.repeated_);

    if (!isPresent(id)) {
      return std::nullopt;
    }
    auto it = desrial_item_map_.find(id);
    if (it == desrial_item_map_.end()) {
      /*
    // offset of heaer<len, bytes....>
    auto offset = calcOffset(id);
    auto bytes_base = buf_ + offset + 4;
    uint32_t len = *reinterpret_cast<const uint32_t *>(buf_ + offset);

    // assert(offset + 1 <= buf_len_);
    desrial_item_map_.insert({id, new ByteSingleMeta(bytes_base, len)});
    */
      decodeAndStoreNonPrimitiveList(id, mem_sym);
      it = desrial_item_map_.find(id);
    }
    return dynamic_cast<ByteRepeatedMeta *>(it->second)->get(idx);
  }

  // no of times an element is repeated
  auto size(const std::string &mem_name) -> uint32_t {
    uint32_t id = symbol_->get_member_id(mem_name);
    assert(id <= symbol_->members_.back().id_);
    auto mem_sym = symbol_->members_[id - 1];
    assert(mem_sym.repeated_);
    if (!isPresent(id)) {
      return 0;
    }
    auto it = desrial_item_map_.find(id);
    if (!TypeHelper::isComplexType(mem_sym.type_)) {
      if (it == desrial_item_map_.end()) {
        // offset of heaer<listOffset, size>
        auto offset = calcOffset(id);
        auto hdr_base = buf_ + offset;
        // assert(offset + 1 <= buf_len_);
        decodeAndStorePrimitiveList(hdr_base, mem_sym);
        it = desrial_item_map_.find(id);
      }
      return dynamic_cast<PrimitiveRepeatedMeta *>(it->second)->size();
    }

    // Complex, Repeated
    if (it == desrial_item_map_.end()) {
      decodeAndStoreNonPrimitiveList(id, mem_sym);
      it = desrial_item_map_.find(id);
    }
    return dynamic_cast<ByteRepeatedMeta *>(it->second)->size();
  }

private:
  void decodeAndStorePrimitiveList(const char *hdr_base, const Symbol &sym) {
    // at hdr_base, offset from the starting
    auto item_base = reinterpret_cast<const char *>(
        reinterpret_cast<gen_ptr_t>(hdr_base + 4));
    uint32_t size = *reinterpret_cast<const uint32_t *>(
        reinterpret_cast<gen_ptr_t>(hdr_base));
    desrial_item_map_.insert(
        {sym.id_, new PrimitiveRepeatedMeta(item_base, size,
                                            TypeHelper::typeSize(sym.type_))});
  }
  void decodeAndStoreNonPrimitiveList(uint32_t id, Symbol &mem_sym) {
    auto offset = calcOffset(id);
    auto hdr_base = buf_ + offset;
    uint32_t size = *reinterpret_cast<const uint32_t *>(
        reinterpret_cast<gen_ptr_t>(hdr_base));
    auto item_base = reinterpret_cast<const char *>(
        reinterpret_cast<gen_ptr_t>(hdr_base + 8));
    desrial_item_map_.insert(
        {mem_sym.id_, new ByteRepeatedMeta(item_base, size)});
  }
  void init() {
    // Extract bitmap
    bitmap_ = buf_[0];
    std::cout << "bitmap: " << bitmap_ << std::endl;
  }
  inline uint32_t getMask(const uint32_t id) {
    assert(id > 0 && id < 32);
    return 1 << (id - 1);
  }
  inline bool isPresent(const uint32_t id) {
    return static_cast<bool>(bitmap_ & getMask(id));
  }

  /*
   * For list type object, it returns the list header offset
   */
  size_t calcOffset(const uint32_t id) {
    size_t offset = 4;
    for (uint32_t id_iter = 1; id_iter <= 32; id_iter++) {
      if (!isPresent(id_iter)) {
        continue;
      }
      auto &mem_sym = symbol_->members_[id_iter - 1];
      // For nested message, handle separately
      // Simple, non-repeated
      if (!TypeHelper::isComplexType(mem_sym.type_) && !mem_sym.repeated_) {
        if (id == id_iter) {
          return offset;
        }
        offset += TypeHelper::typeSize(mem_sym.type_);
      }
      // Simple, repeated
      if (!TypeHelper::isComplexType(mem_sym.type_) && mem_sym.repeated_) {
        if (id == id_iter) {
          return offset;
        }
        uint32_t size = *(uint32_t *)(buf_ + offset);
        offset = offset + 4 + size * TypeHelper::typeSize(mem_sym.type_);
      }
      // Bytes, non-repeated
      if (TypeHelper::isComplexType(mem_sym.type_) && !mem_sym.repeated_) {
        if (id == id_iter) {
          return offset;
        }
        auto len = *reinterpret_cast<const uint32_t *>(buf_ + offset);
        offset = offset + 4 + len;
      }

      // Bytes, repeated
      if (TypeHelper::isComplexType(mem_sym.type_) && mem_sym.repeated_) {
        if (id == id_iter) {
          return offset;
        }
        // <size, listLen>
        auto list_start = offset + 8;
        // *reinterpret_cast<const uint32_t *>(buf_ + offset + 8);
        auto list_len = *reinterpret_cast<const uint32_t *>(buf_ + offset + 4);
        offset = list_start + list_len;
      }
    }
    return offset;
  }
  std::unordered_map<uint32_t, DesrialiserMetaBase *> desrial_item_map_;
  const std::string message_name_{};
  const Symbol *symbol_{nullptr};
  const char *buf_{nullptr};
  size_t buf_len_{0};
  uint32_t bitmap_{0};
};
} // namespace user
