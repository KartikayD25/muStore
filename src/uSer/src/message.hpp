#pragma once

#include <cstring>
#include <unordered_map>
#include <vector>

#include "define.hpp"
#include "symtab.hpp"

// #include "deserialiser.hpp"

#include "latency_tool.hpp"
#include "helper.hpp"
#include "net/net.hpp"

#define SET_VAL(type)                                                          \
  inline void setVal(type val) { val_ = static_cast<gen_ptr_t>(val); }

#define ADD_VAL(type)                                                          \
  inline void addVal(type val) {                                               \
    values_.push_back(static_cast<gen_ptr_t>(val));                            \
  }

namespace user {

class FieldValue {
public:
  virtual void serialize() = 0;
  virtual const Symbol &getSymbol() = 0;
};

/* Avoid OOPs for better runtime performance */

class SingleSimpleFieldValue : public FieldValue {
public:
  SingleSimpleFieldValue() = delete;
  SingleSimpleFieldValue(const Symbol &symbol) : symbol_(symbol) {}
  SingleSimpleFieldValue(SingleSimpleFieldValue &) = delete;

  void setVal(void *val) {
    memcpy(data_, val, TypeHelper::typeSize(symbol_.type_));
  }
  template <typename T> inline auto getVal() -> T { return *(T *)data_; }

  inline auto getBasePtr() -> void * { return (void *)data_; }

  virtual void serialize() {
    std::cerr << LINE_LOCATION << "TODO: Implement" << std::endl;
  }
  virtual const Symbol &getSymbol() { return symbol_; }

private:
  char data_[8] = {0};
  const Symbol &symbol_;
};

class RepeatedSimpleFieldValue : public FieldValue {
public:
  friend class Message;
  friend class Person;
  RepeatedSimpleFieldValue() = delete;
  RepeatedSimpleFieldValue(const Symbol &symbol) : symbol_(symbol) {}
  RepeatedSimpleFieldValue(RepeatedSimpleFieldValue &) = delete;

  void clear() { values_.clear(); }
  size_t size() { return values_.size(); }

  ADD_VAL(gen_ptr_t);
  /*ADD_VAL(bool *);
  ADD_VAL(int32_t *);
  ADD_VAL(int64_t *);
  ADD_VAL(uint32_t *);
  ADD_VAL(uint64_t *);
  ADD_VAL(float *);
*/
  inline gen_ptr_t getVal(size_t idx) {
    if (idx < values_.size()) {
      return values_[idx];
    }
    return nullptr;
  }
  virtual void serialize() {
    std::cerr << LINE_LOCATION << "TODO: Implement" << std::endl;
  }
  virtual const Symbol &getSymbol() { return symbol_; }

private:
  std::vector<gen_ptr_t> values_;
  const Symbol &symbol_;
  uint32_t list_size_{0};
};

class SingleComplexFieldValue : public FieldValue {
public:
  friend class Message;
  friend class Person;
  SingleComplexFieldValue() = delete;
  SingleComplexFieldValue(const Symbol &symbol) : symbol_(symbol) {}
  SingleComplexFieldValue(const Symbol &symbol, std::string &val)
      : symbol_(symbol) {
    setVal(val);
  }
  SingleComplexFieldValue(const Symbol &symbol, const char *val)
      : symbol_(symbol) {
    setVal(val);
  }
  SingleComplexFieldValue(const Symbol &symbol, const char *val, size_t len)
      : symbol_(symbol) {
    setVal(val, len);
  }

  inline void setVal(std::string &val) { setVal(val.data(), val.length()); }
  inline void setVal(const std::string &val) { setVal(val.data(), val.length()); }

  inline void setVal(const char *val) { setVal(val, strlen(val)); }
  inline void setVal(const char *val, size_t len) {
    val_ = val;
    len_ = len;
  }

  inline std::string getValStr() { return std::string(getValByte()); }
  inline const char *getValByte() { return static_cast<const char *>(val_); }

  virtual void serialize() {
    std::cerr << LINE_LOCATION << "TODO: Implement" << std::endl;
  }
  virtual const Symbol &getSymbol() { return symbol_; }

private:
  gen_ptr_t val_{nullptr};
  uint32_t len_{0};
  const Symbol &symbol_;
};

class RepeatedComplexFieldValue : public FieldValue {
public:
  friend class Message;
  friend class Person;
  RepeatedComplexFieldValue() = delete;
  RepeatedComplexFieldValue(const Symbol &symbol) : symbol_(symbol) {}
  RepeatedComplexFieldValue(RepeatedComplexFieldValue &) = delete;

  void clear() {
    values_.clear();
    len_ = 0;
  }
  size_t size() { return values_.size(); }

  void addVal(std::string &val) {
    values_.push_back(SingleComplexFieldValue(symbol_, val));
    len_ = values_.size();
  }
  void addVal(const char *val) {
    values_.push_back(SingleComplexFieldValue(symbol_, val));
    len_ = values_.size();
  }
  void addVal(const char *val, size_t len) {
    values_.push_back(SingleComplexFieldValue(symbol_, val, len));
    len_ = values_.size();
  }

  std::string getValStr(size_t idx) {
    if (idx < size()) {
      return values_[idx].getValStr();
    }
    return "";
  }
  const char *getValByte(size_t idx) {
    if (idx < size()) {
      return values_[idx].getValByte();
    }
    return nullptr;
  }
  virtual void serialize() {
    std::cerr << LINE_LOCATION << "TODO: Implement" << std::endl;
  }
  virtual const Symbol &getSymbol() { return symbol_; }

private:
  std::vector<SingleComplexFieldValue> values_;
  uint32_t len_{0};
  const Symbol &symbol_;
  /*
   * 0: len of the list
   * 1: len of list: each item header + item length.Used while serialising
   *    Used to directly go to next id member
   */
  uint32_t list_header_[2] = {0};
};

class Message {
public:
  Message(std::string &message_name)
      : message_name_(message_name), symbol_(SYMTAB->getSymbol(message_name_)) {
    init();
  }

  /* Set */
  // Simple, non-repeated
  void setVal(const std::string &mem_name, uint32_t val) {
    setVal(mem_name, (void *)&val);
  }
  void setVal(const std::string &mem_name, uint64_t val) {
    setVal(mem_name, (void *)&val);
  }

  void setVal(const std::string &mem_name, int32_t val) {
    setVal(mem_name, (void *)&val);
  }
  void setVal(const std::string &mem_name, int64_t val) {
    setVal(mem_name, (void *)&val);
  }

  void setVal(const std::string &mem_name, float val) {
    setVal(mem_name, (void *)&val);
  }
  void setVal(const std::string &mem_name, double val) {
    setVal(mem_name, (void *)&val);
  }

/*
  template<typename T>
  void set_field(const T& val){
    static_assert(TypeHelper::IsValidUserType<T>::value,
                 "Invalid type for field");
    
    uint32_t id = symbol_->get_member_id(field);  
    uint32_t idx = id - 1;
    
    static_assert(!TypeHelper::IsComplexType<T>::value,
                 "Cannot use set_field with complex types");
                 
    static_assert(TypeHelper::isSameType<T>(symbol_->members_[idx].type_),
                 "Type mismatch between field and value");
  }


*/


private:
  // Simple, non-repeated
  void setVal(const std::string &mem_name, void *val) {
    uint32_t id = symbol_->get_member_id(mem_name);
    uint32_t idx = id - 1;
    assert(!TypeHelper::isComplexType(symbol_->members_[idx].type_) &&
           !symbol_->members_[idx].repeated_);
    auto field = dynamic_cast<SingleSimpleFieldValue *>(values_[idx]);
    assert(field);
    field->setVal(val);
    bitmap_ |= getMask(id);
  }

public:
  // Complex, non-repeated
  void setVal(const std::string &mem_name, std::string &val) {
    uint32_t id = symbol_->get_member_id(mem_name);
    uint32_t idx = id - 1;
    assert(TypeHelper::isComplexType(symbol_->members_[idx].type_) &&
           !symbol_->members_[idx].repeated_);
    auto field = dynamic_cast<SingleComplexFieldValue *>(values_[idx]);
    field->setVal(val);
    bitmap_ |= getMask(id);
  }
  // Complex, non-repeated
  void setVal(const std::string &mem_name, const char *val) {
    uint32_t id = symbol_->get_member_id(mem_name);
    uint32_t idx = id - 1;
    assert(TypeHelper::isComplexType(symbol_->members_[idx].type_) &&
           !symbol_->members_[idx].repeated_);
    auto field = dynamic_cast<SingleComplexFieldValue *>(values_[idx]);
    field->setVal(val);
    bitmap_ |= getMask(id);
  }
  // Complex, non-repeated
  void setVal(const std::string &mem_name, const char *val, size_t len) {
    uint32_t id = symbol_->get_member_id(mem_name);
    uint32_t idx = id - 1;
    assert(TypeHelper::isComplexType(symbol_->members_[idx].type_) &&
           !symbol_->members_[idx].repeated_);
    auto field = dynamic_cast<SingleComplexFieldValue *>(values_[idx]);
    field->setVal(val, len);
    bitmap_ |= getMask(id);
  }

  /* Add */

  // Simple, repeated
  void addVal(const std::string &mem_name, gen_ptr_t val) {
    uint32_t id = symbol_->get_member_id(mem_name);
    uint32_t idx = id - 1;
    assert(!TypeHelper::isComplexType(symbol_->members_[idx].type_) &&
           symbol_->members_[idx].repeated_);
    auto field = dynamic_cast<RepeatedSimpleFieldValue *>(values_[idx]);
    assert(field);
    field->addVal(val);
    bitmap_ |= getMask(id);
    repeated_bitmap_ |= getMask(id);
  }
  // Complex, repeated
  void addVal(const std::string &mem_name, std::string &val) {
    uint32_t id = symbol_->get_member_id(mem_name);
    uint32_t idx = id - 1;
    assert(TypeHelper::isComplexType(symbol_->members_[idx].type_) &&
           symbol_->members_[idx].repeated_);
    auto field = dynamic_cast<RepeatedComplexFieldValue *>(values_[idx]);
    field->addVal(val);
    bitmap_ |= getMask(id);
    repeated_bitmap_ |= getMask(id);
  }

  // Complex, repeated
  void addVal(const std::string &mem_name, const char *val) {
    uint32_t id = symbol_->get_member_id(mem_name);
    uint32_t idx = id - 1;
    assert(TypeHelper::isComplexType(symbol_->members_[idx].type_) &&
           symbol_->members_[idx].repeated_);
    auto field = dynamic_cast<RepeatedComplexFieldValue *>(values_[idx]);
    field->addVal(val);
    bitmap_ |= getMask(id);
    repeated_bitmap_ |= getMask(id);
  }
  // Complex, repeated
  void addVal(const std::string &mem_name, const char *val, size_t len) {
    uint32_t id = symbol_->get_member_id(mem_name);
    uint32_t idx = id - 1;
    assert(TypeHelper::isComplexType(symbol_->members_[idx].type_) &&
           symbol_->members_[idx].repeated_);
    auto field = dynamic_cast<RepeatedComplexFieldValue *>(values_[idx]);
    field->addVal(val, len);
    bitmap_ |= getMask(id);
    repeated_bitmap_ |= getMask(id);
  }

  /* Get */

  // Simple, non-repeated
  inline gen_ptr_t getVal(const std::string &mem_name) {
    uint32_t id = symbol_->get_member_id(mem_name);
    auto mem_sym = symbol_->members_[id - 1];
    assert(!TypeHelper::isComplexType(symbol_->members_[id - 1].type_) &&
           !mem_sym.repeated_);
    if (!isPresent(id)) {
      return nullptr;
    }
    return dynamic_cast<SingleSimpleFieldValue *>(values_[id - 1])
        ->getBasePtr();
  }
  // Simple, repeated
  inline gen_ptr_t getVal(const std::string &mem_name, size_t idx) {
    uint32_t id = symbol_->get_member_id(mem_name);
    auto mem_sym = symbol_->members_[id - 1];
    assert(!TypeHelper::isComplexType(symbol_->members_[id - 1].type_) &&
           mem_sym.repeated_);
    if (!isPresent(id)) {
      return nullptr;
    }
    return dynamic_cast<RepeatedSimpleFieldValue *>(values_[id - 1])
        ->getVal(idx);
  }
  // Complex, non-repeated
  inline std::string getValStr(const std::string &mem_name) {
    uint32_t id = symbol_->get_member_id(mem_name);
    auto mem_sym = symbol_->members_[id - 1];
    assert(TypeHelper::isComplexType(symbol_->members_[id - 1].type_) &&
           !mem_sym.repeated_);
    if (!isPresent(id)) {
      return nullptr;
    }
    return dynamic_cast<SingleComplexFieldValue *>(values_[id - 1])
        ->getValStr();
  }
  // Complex, repeated
  inline std::string getValStr(const std::string &mem_name, size_t idx) {
    uint32_t id = symbol_->get_member_id(mem_name);
    auto mem_sym = symbol_->members_[id - 1];
    assert(TypeHelper::isComplexType(symbol_->members_[id - 1].type_) &&
           mem_sym.repeated_);
    if (!isPresent(id)) {
      return nullptr;
    }
    return dynamic_cast<RepeatedComplexFieldValue *>(values_[id - 1])
        ->getValStr(idx);
  }

  /* Clear */

  inline void clear(const std::string &mem_name) {
    uint32_t id = symbol_->get_member_id(mem_name);
    auto mem_sym = symbol_->members_[id - 1];
    assert(mem_sym.repeated_);
    if (TypeHelper::isComplexType(mem_sym.type_)) {
      dynamic_cast<RepeatedComplexFieldValue *>(values_[id - 1])->clear();
    } else {
      dynamic_cast<RepeatedSimpleFieldValue *>(values_[id - 1])->clear();
    }
    bitmap_ &= (~getMask(id));
    repeated_bitmap_ &= (~getMask(id));
  }

  /* Size */

  inline size_t size(const std::string &mem_name) {
    uint32_t id = symbol_->get_member_id(mem_name);
    auto mem_sym = symbol_->members_[id - 1];
    assert(mem_sym.repeated_);
    if (TypeHelper::isComplexType(mem_sym.type_)) {
      return dynamic_cast<RepeatedComplexFieldValue *>(values_[id - 1])->size();
    }
    return dynamic_cast<RepeatedSimpleFieldValue *>(values_[id - 1])->size();
  }

  /* Serialize */

  // serialize message to uSer's wireformat
  char *serialize() {
    std::cerr << LINE_LOCATION << "Not implemented" << std::endl;
    exit(1);
  }

  // Serialize and write
  ssize_t serializeAndWrite(net::Conn *conn, SerializationMethod method) {
    ssize_t bytes = 0;
    if (method == SerializationMethod::COPY) {
      bytes = serializeAndWriteCOPY(conn);
    } else if (method == SerializationMethod::SG) {
      bytes = serializeAndWriteSG(conn);
    } else {
      bytes = serializeAndWriteZC(conn);
    }
    return bytes;
  }

private:
  inline void FreeBuf() {
    free(buf_);
    buf_ = nullptr;
  }
  ssize_t serializeAndWriteCOPY(net::Conn *conn) {
    // Helper::time_unit_t alloc_time = 0, copy_time = 0, write_time = 0,
    //                    dealloc_time = 0, total_time = 0;
    ssize_t bytes = 0;
    size_t offset = 0, buf_size = 0;
    {
      // Helper::Timer<Helper::nanosecond_t> _(alloc_time);
      //buf_size = std::max(static_cast<size_t>(4 + getBufLen()),
      //                    net::common::BUFFER_SIZE);
      buf_size = static_cast<size_t>(4 + getBufLen());
#ifdef TEST_USER
      buf_size = std::max(buf_size,
                          net::common::BUFFER_SIZE);
#endif
      buf_ = (char *)malloc(buf_size); // TODO: Use helper function, calloc
    }
    {
      // Helper::Timer<Helper::nanosecond_t> _(copy_time);
      // bitmap
      memcpy(buf_ + offset, &bitmap_, 4);
      offset += 4;

      for (auto &field_val : values_) {
        const Symbol &sym = field_val->getSymbol();
        uint32_t id = sym.id_;
        if (!isPresent(id)) {
          continue;
        }
        if (sym.type_ == UserTypes::TYPE_MESSAGE) {
          // handle in different way
          NOT_IMPLEMENTED;
        }
        // Simple, single
        if (!TypeHelper::isComplexType(sym.type_) && !sym.repeated_) {
          auto value = dynamic_cast<SingleSimpleFieldValue *>(field_val);
          memcpy(buf_ + offset, value->getBasePtr(),
                 TypeHelper::typeSize(sym.type_));
          offset += TypeHelper::typeSize(sym.type_);
        }
        // Simple, repeated
        if (!TypeHelper::isComplexType(sym.type_) && sym.repeated_) {
          auto value = dynamic_cast<RepeatedSimpleFieldValue *>(field_val);
          value->list_size_ = static_cast<uint32_t>(value->size());

          memcpy(buf_ + offset, (void *)&(value->list_size_), 4);
          offset += 4;

          // Copy items
          // for (auto &val_iter : value->values_) {
          for (auto val_iter : value->values_) {
            memcpy(buf_ + offset, (val_iter),
                   TypeHelper::typeSize(value->getSymbol().type_));
            offset += TypeHelper::typeSize(value->getSymbol().type_);
          }
        }
        // Complex, nonrepeated
        if (TypeHelper::isComplexType(sym.type_) && !sym.repeated_) {
          auto value = dynamic_cast<SingleComplexFieldValue *>(field_val);
          // <len, bytes...>
          // length
          memcpy(buf_ + offset, (void *)&(value->len_), 4);
          offset += 4;

          memcpy(buf_ + offset, (void *)(value->val_), value->len_);
          offset += value->len_;
        }
        // Complex, repeated
        if (TypeHelper::isComplexType(sym.type_) && sym.repeated_) {
          auto value = dynamic_cast<RepeatedComplexFieldValue *>(field_val);
          // value->list_size_ = value->values_.size();
          // TODO: Add constraints on no of bytes
          // <size, bytes...>
          // size
          value->list_header_[0] = value->len_;
          value->list_header_[1] = 0;

          auto list_len_offset = offset + 4;
          // offload the whole 8 byte. At the end, update the list len
          memcpy(buf_ + offset, (void *)(value->list_header_),
                 sizeof(value->list_header_));
          offset += sizeof(value->list_header_);

          // Copy items
          for (auto &val_iter : value->values_) {
            memcpy(buf_ + offset, (void *)(&val_iter.len_), 4);
            offset += 4;
            value->list_header_[1] += 4;
            // std::cout << "Data copy offset " << offset << std::endl;
            memcpy(buf_ + offset, (void *)val_iter.val_, val_iter.len_);
            offset += val_iter.len_;
            value->list_header_[1] += val_iter.len_;
          }
          // Copy the list len
          memcpy(buf_ + list_len_offset, (void *)&(value->list_header_[1]),
                 sizeof(value->list_header_[1]));
        }
      }
    }
    std::cout << "Offset: " << offset << std::endl;
    {
      // Helper::Timer<Helper::nanosecond_t> _(write_time);
      bytes = conn->Write(buf_, buf_size);
    }
    {
      // Helper::Timer<Helper::nanosecond_t> _(dealloc_time);
      free(buf_);
    }
    // total_time += alloc_time + copy_time + write_time + dealloc_time;
    // user::LatencyRecorderMgr::Get()->AddMetric(LatencyRecorderMgr::Metric(
    //    alloc_time, copy_time, write_time, dealloc_time, total_time));
    return bytes;
  }

  ssize_t serializeAndWriteSG(net::Conn *conn) {
    // bitmap, garbage
#ifdef TEST_USER
    char garbage_buf[net::common::BUFFER_SIZE] = {'\0'};
#endif
    ssize_t bytes = 0;
    uint32_t offset = 0;
    uint32_t iov_len = 0, iov_idx = 0;
    Helper::time_unit_t prep_time = 0, prep_time1 = 0, write_time = 0,
                       total_time = 0;
    {
      Helper::Timer<Helper::nanosecond_t> _(prep_time1);
      iov_len = 1 + getIOVecLen(); // + 1 -- for garbage; isn't it already added below ?  
#ifdef TEST_USER
      iov_len += 1; // for garbage;
#endif
    }
    struct iovec iov[iov_len];
    {
      Helper::Timer<Helper::nanosecond_t> _(prep_time);
      // bitmap
      iov[iov_idx].iov_base = (void *)(&bitmap_);
      iov[iov_idx++].iov_len = sizeof(bitmap_);
      offset += sizeof(bitmap_);

      for (auto &field_val : values_) {
        const Symbol &sym = field_val->getSymbol();
        uint32_t id = sym.id_;
        if (!isPresent(id)) {
          continue;
        }
        if (sym.type_ == UserTypes::TYPE_MESSAGE) {
          // handle in different way
          NOT_IMPLEMENTED;
        }
        // Simple, single
        if (!TypeHelper::isComplexType(sym.type_) && !sym.repeated_) {
          auto value = dynamic_cast<SingleSimpleFieldValue *>(field_val);
          iov[iov_idx].iov_base = value->getBasePtr();
          iov[iov_idx++].iov_len =
              TypeHelper::typeSize(value->getSymbol().type_);
          offset += TypeHelper::typeSize(value->getSymbol().type_);
        }
        // Simple, repeated
        if (!TypeHelper::isComplexType(sym.type_) && sym.repeated_) {
          auto value = dynamic_cast<RepeatedSimpleFieldValue *>(field_val);
          // TODO: Add constraints on no of repeated fields
          value->list_size_ = static_cast<uint32_t>(value->size());

          iov[iov_idx].iov_base = (void *)&(value->list_size_);
          iov[iov_idx++].iov_len = 4;
          offset += 4;

          // Copy items
          for (auto &val_iter : value->values_) {
            iov[iov_idx].iov_base = (void *)val_iter;
            iov[iov_idx++].iov_len =
                TypeHelper::typeSize(value->getSymbol().type_);
            offset += TypeHelper::typeSize(value->getSymbol().type_);
          }
        }

        // Complex, nonrepeated
        if (TypeHelper::isComplexType(sym.type_) && !sym.repeated_) {
          auto value = dynamic_cast<SingleComplexFieldValue *>(field_val);
          // TODO: Add constraints on no of bytes
          // <len, bytes...>
          // length
          iov[iov_idx].iov_base = reinterpret_cast<void *>(&(value->len_));
          iov[iov_idx++].iov_len = sizeof(value->len_);
          offset += sizeof(value->len_);

          iov[iov_idx].iov_base = const_cast<void *>(value->val_);
          iov[iov_idx++].iov_len = value->len_;
          offset += value->len_;
        }

        // Complex, repeated
        if (TypeHelper::isComplexType(sym.type_) && sym.repeated_) {
          auto value = dynamic_cast<RepeatedComplexFieldValue *>(field_val);
          // value->list_size_ = value->values_.size();
          // TODO: Add constraints on no of bytes
          // <size, bytes...>
          // size
          /*
          iov[iov_idx].iov_base =
              reinterpret_cast<void *>(&(value->list_size_));
          iov[iov_idx++].iov_len = 4;
          offset += 4;
          */

          iov[iov_idx].iov_base =
              reinterpret_cast<void *>((value->list_header_)); // Removed &
          iov[iov_idx++].iov_len = sizeof(value->list_header_);
          offset += sizeof(value->list_header_);

          value->list_header_[0] = value->len_;
          value->list_header_[1] = 0;

          // Copy items
          for (auto &val_iter : value->values_) {
            iov[iov_idx].iov_base = reinterpret_cast<void *>(&val_iter.len_);
            iov[iov_idx++].iov_len = sizeof(val_iter.len_);
            offset += sizeof(val_iter.len_);
            value->list_header_[1] += sizeof(val_iter.len_);

            iov[iov_idx].iov_base = const_cast<void *>(val_iter.val_);
            iov[iov_idx++].iov_len = val_iter.len_;
            offset += val_iter.len_;
            value->list_header_[1] += val_iter.len_;
          }
        }
      }
    }
#ifdef TEST_USER
    iov[iov_len - 1].iov_base = (void *)garbage_buf;
    iov[iov_len - 1].iov_len = net::common::BUFFER_SIZE - offset;
    offset += iov[iov_len - 1].iov_len;
    std::cout << "OFFSET: " << offset << " BUFFF: " << net::common::BUFFER_SIZE << std::endl;
#endif
    assert(offset == net::common::BUFFER_SIZE);
    {
      Helper::Timer<Helper::nanosecond_t> _(write_time);
      bytes = conn->Writev(iov, iov_len);
    }
    prep_time += prep_time1;
    total_time = prep_time + write_time;
    LatencyRecorderMgr::Get()->AddMetric(
       LatencyRecorderMgr::Metric(prep_time, write_time, total_time));
    return bytes;
  }

  ssize_t serializeAndWriteZC(net::Conn *conn) { //implement zerocopy here 
#ifdef TEST_USER
    char garbage_buf[net::common::BUFFER_SIZE] = {'\0'};
#endif
    ssize_t bytes = 0;
    uint32_t offset = 0;
    uint32_t iov_len = 0, iov_idx = 0;
    struct msghdr msg = {0};
    Helper::time_unit_t prep_time = 0, prep_time1 = 0, write_time = 0,
                       total_time = 0;
    {
      Helper::Timer<Helper::nanosecond_t> _(prep_time1);
      iov_len = 1 + getIOVecLen(); // + 1 -- for garbage; isn't it already added below ?  
#ifdef TEST_USER
      iov_len += 1; // for garbage;
#endif
    }
    struct iovec iov[iov_len];
    {
      Helper::Timer<Helper::nanosecond_t> _(prep_time);
      // bitmap
      iov[iov_idx].iov_base = (void *)(&bitmap_);
      iov[iov_idx++].iov_len = sizeof(bitmap_);
      offset += sizeof(bitmap_);

      for (auto &field_val : values_) {
        const Symbol &sym = field_val->getSymbol();
        uint32_t id = sym.id_;
        if (!isPresent(id)) {
          continue;
        }
        if (sym.type_ == UserTypes::TYPE_MESSAGE) {
          // handle in different way
          NOT_IMPLEMENTED;
        }
        // Simple, single
        if (!TypeHelper::isComplexType(sym.type_) && !sym.repeated_) {
          auto value = dynamic_cast<SingleSimpleFieldValue *>(field_val);
          iov[iov_idx].iov_base = value->getBasePtr();
          iov[iov_idx++].iov_len =
              TypeHelper::typeSize(value->getSymbol().type_);
          offset += TypeHelper::typeSize(value->getSymbol().type_);
        }
        // Simple, repeated
        if (!TypeHelper::isComplexType(sym.type_) && sym.repeated_) {
          auto value = dynamic_cast<RepeatedSimpleFieldValue *>(field_val);
          // TODO: Add constraints on no of repeated fields
          value->list_size_ = static_cast<uint32_t>(value->size());

          iov[iov_idx].iov_base = (void *)&(value->list_size_);
          iov[iov_idx++].iov_len = 4;
          offset += 4;

          // Copy items
          for (auto &val_iter : value->values_) {
            iov[iov_idx].iov_base = (void *)val_iter;
            iov[iov_idx++].iov_len =
                TypeHelper::typeSize(value->getSymbol().type_);
            offset += TypeHelper::typeSize(value->getSymbol().type_);
          }
        }

        // Complex, nonrepeated
        if (TypeHelper::isComplexType(sym.type_) && !sym.repeated_) {
          auto value = dynamic_cast<SingleComplexFieldValue *>(field_val);
          // TODO: Add constraints on no of bytes
          // <len, bytes...>
          // length
          iov[iov_idx].iov_base = reinterpret_cast<void *>(&(value->len_));
          iov[iov_idx++].iov_len = sizeof(value->len_);
          offset += sizeof(value->len_);

          iov[iov_idx].iov_base = const_cast<void *>(value->val_);
          iov[iov_idx++].iov_len = value->len_;
          offset += value->len_;
        }

        // Complex, repeated
        if (TypeHelper::isComplexType(sym.type_) && sym.repeated_) {
          auto value = dynamic_cast<RepeatedComplexFieldValue *>(field_val);
          // value->list_size_ = value->values_.size();
          // TODO: Add constraints on no of bytes
          // <size, bytes...>
          // size
          /*
          iov[iov_idx].iov_base =
              reinterpret_cast<void *>(&(value->list_size_));
          iov[iov_idx++].iov_len = 4;
          offset += 4;
          */

          iov[iov_idx].iov_base =
              reinterpret_cast<void *>((value->list_header_)); // Removed &
          iov[iov_idx++].iov_len = sizeof(value->list_header_);
          offset += sizeof(value->list_header_);

          value->list_header_[0] = value->len_;
          value->list_header_[1] = 0;

          // Copy items
          for (auto &val_iter : value->values_) {
            iov[iov_idx].iov_base = reinterpret_cast<void *>(&val_iter.len_);
            iov[iov_idx++].iov_len = sizeof(val_iter.len_);
            offset += sizeof(val_iter.len_);
            value->list_header_[1] += sizeof(val_iter.len_);

            iov[iov_idx].iov_base = const_cast<void *>(val_iter.val_);
            iov[iov_idx++].iov_len = val_iter.len_;
            offset += val_iter.len_;
            value->list_header_[1] += val_iter.len_;
          }
        }
      }
      //only additions to SG method 
      msg.msg_iov = iov;
      msg.msg_iovlen = iov_len;
    }
#ifdef TEST_USER
    iov[iov_len - 1].iov_base = (void *)garbage_buf;
    iov[iov_len - 1].iov_len = net::common::BUFFER_SIZE - offset;
    offset += iov[iov_len - 1].iov_len;
    std::cout << "OFFSET: " << offset << " BUFFF: " << net::common::BUFFER_SIZE << std::endl;
#endif
    assert(offset == net::common::BUFFER_SIZE); //why is this here?? 
    {
      Helper::Timer<Helper::nanosecond_t> _(write_time);
      bytes = conn->sendmsg_ZC(&msg);
    }
    return bytes;
  }

  uint32_t getIOVecLen() {
    uint32_t len = 0; //__builtin_popcount(bitmap_); //
    for (uint32_t id = 1; id <= 32; id++) {
      if (!isPresent(id)) {
        continue;
      }
      auto mem_sym = symbol_->members_[id - 1];
      // Simple non-repeated
      if (!TypeHelper::isComplexType(mem_sym.type_) && !mem_sym.repeated_) {
        len += 1; // only item
      }
      // Simple repeated
      if (!TypeHelper::isComplexType(mem_sym.type_) && mem_sym.repeated_) {
        // List Header + list items
        len +=
            1 +
            dynamic_cast<RepeatedSimpleFieldValue *>(values_[id - 1])->size();
      }
      // Complex non-repeated
      if (TypeHelper::isComplexType(mem_sym.type_) && !mem_sym.repeated_) {
        // Header, bytes
        len += 2;
      }
      // Complex repeated
      if (TypeHelper::isComplexType(mem_sym.type_) && mem_sym.repeated_) {
        // List Header + 2 * list items
        len +=
            1 + 2 * dynamic_cast<RepeatedComplexFieldValue *>(values_[id - 1])
                        ->size();
      }
    }
    return len;
  }

  uint32_t getBufLen() {
    uint32_t len = 0; //__builtin_popcount(bitmap_); //
    for (uint32_t id = 1; id <= 32; id++) {
      if (!isPresent(id)) {
        continue;
      }
      auto mem_sym = symbol_->members_[id - 1];
      // Simple non-repeated
      if (!TypeHelper::isComplexType(mem_sym.type_) && !mem_sym.repeated_) {
        len += TypeHelper::typeSize(mem_sym.type_); // only item
      }
      // Simple repeated
      if (!TypeHelper::isComplexType(mem_sym.type_) && mem_sym.repeated_) {
        // List Header + list items
        len +=
            4 +
            dynamic_cast<RepeatedSimpleFieldValue *>(values_[id - 1])->size() *
                TypeHelper::typeSize(mem_sym.type_);
      }
      // Complex non-repeated
      if (TypeHelper::isComplexType(mem_sym.type_) && !mem_sym.repeated_) {
        // Header, bytes
        len +=
            4 + dynamic_cast<SingleComplexFieldValue *>(values_[id - 1])->len_;
      }
      // Complex repeated
      if (TypeHelper::isComplexType(mem_sym.type_) && mem_sym.repeated_) {
        // List Header + 2 * list items
        // len +=
        //    4 + 4 * dynamic_cast<RepeatedComplexFieldValue *>(values_[id -
        //    1])->size();
        len += 4 + 4; // <size, Len>
        auto val = dynamic_cast<RepeatedComplexFieldValue *>(values_[id - 1]);
        for (auto scfv : val->values_) {
          len += 4 + scfv.len_;
        }
      }
    }
    return len;
  }

  void init() {
    assert(symbol_ != nullptr);  
    for (const auto &mem_sym : symbol_->members_) {
      FieldValue *val = nullptr;
      if (TypeHelper::isComplexType(mem_sym.type_)) {
        if (mem_sym.repeated_) {
          val = new RepeatedComplexFieldValue(mem_sym);
        } else {
          val = new SingleComplexFieldValue(mem_sym);
        }
      } else {
        if (mem_sym.repeated_) {
          val = new RepeatedSimpleFieldValue(mem_sym);
        } else {
          val = new SingleSimpleFieldValue(mem_sym);
        }
      }
      values_.push_back(val);
    }
  }

  
  inline uint32_t getMask(const uint32_t id) {
    assert(id > 0 && id < 33);
    return 1 << (id - 1);
  }
  inline bool isPresent(const uint32_t id) {
    return static_cast<bool>(bitmap_ & getMask(id));
  }
  inline bool isRepeated(const uint32_t id) {
    return static_cast<bool>(repeated_bitmap_ & getMask(id));
  }
  char *buf_{nullptr};
  const std::string message_name_{};
  const Symbol *symbol_{nullptr};
  std::vector<FieldValue *> values_;  
  uint32_t bitmap_{0};
  uint32_t repeated_bitmap_{0}; // repeated elements that are being repeated
};

class InitMessage {
public:
  static Message *allocateMessage(std::string message_name) {
    return new Message(message_name);
  }

  // static Deserialiser *getDeserialiserPtr(std::string message_name, char *buf,
  //                                         size_t len) {
  //   return new Deserialiser(message_name, buf, len);
  // }
};
} // namespace user
