#pragma once

#include <algorithm>
#include <cassert>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <vector>

#include "define.hpp"
#include "parser/parser.hpp"
#include "types.hpp"
#include "user.hpp"

namespace user {

class Symbol {
public:
  const bool message_{false}; // Message or member
  const uint32_t id_{0};
  const std::string name_;
  const bool repeated_{false};
  Symbol *parent_{nullptr};
  UserTypes type_{UserTypes::TYPE_UNKNOWN};
  Symbol *nestedType_{nullptr};
  const bool optional_{false};
  std::vector<Symbol> members_;
  std::unordered_map<std::string, uint32_t> member_name_to_id_;
  uint32_t max_id_{0};
  static uint32_t msg_id_cnt_;

  bool operator==(const Symbol &sym) {
    if (message_ && name_ == sym.name_) {
      assert(type_ == UserTypes::TYPE_MESSAGE);
      assert(sym.type_ == UserTypes::TYPE_MESSAGE);
      assert(members_.size() == sym.members_.size());
      return true;
    }
    return name_ == sym.name_ && id_ == sym.id_;
  }

  /* Builder APIs */

  // Message Members
  Symbol(uint32_t id, const std::string name, bool repeated, Symbol *parent,
         UserTypes type, bool optional)
      : id_(id), name_(name), repeated_(repeated), parent_(parent), type_(type),
        optional_(optional) {}

  // Message Members of nested type
  Symbol(uint32_t id, const std::string name, bool repeated, Symbol *parent,
         UserTypes type, Symbol *nestedType, bool optional)
      : id_(id), name_(name), repeated_(repeated), parent_(parent), type_(type),
        nestedType_(nestedType), optional_(optional) {
    assert(nestedType->message_);
  }

  // For Message
  Symbol(const std::string name)
      : message_{true}, id_{++msg_id_cnt_}, name_(name),
        type_(UserTypes::TYPE_MESSAGE) {}

  std::string toString() const {
    std::stringstream ss;
    if (message_) {
      ss << name_ << " {";
      for (auto mem : members_) {
        if (mem.repeated_) {
          ss << " repeated";
        }
        if (mem.optional_) {
          ss << " optional";
        }
        ss << " " << TypeHelper::typeToStr(mem.type_) << " " << mem.name_
           << " = " << mem.id_ << ";";
      }
      ss << " }";
    } else {
      if (repeated_) {
        ss << " repeated";
      }
      if (optional_) {
        ss << " optional";
      }
      ss << TypeHelper::typeToStr(type_) << " " << name_ << " =  "
         << std::to_string(id_) + " ...Parent: " + parent_->name_;
    }
    return ss.str();
  }

  /*
   * Must push elements in sequential order
   */
  bool addMember(Symbol &symbol) {
    assert(message_);
    assert(!symbol.message_);

    // Check same name or same id exist
    auto it = std::find_if(
        members_.begin(), members_.end(), [&symbol](const Symbol &sym) {
          return sym.name_ == symbol.name_ || sym.id_ == symbol.id_;
        });
    if (it != members_.end()) {
      std::cerr << LINE_LOCATION << "member already present. " << symbol
                << " Found Symbol: " << *it << std::endl;
      exit(1);
    }
    // Check continuity
    if ((members_.size() + 1 != symbol.id_)) {
      std::cerr << LINE_LOCATION << "Insertion must be continous. "
                << "Message: " << symbol.parent_->name_
                << " Member: " << symbol.name_ << " Expecting "
                << (members_.size() + 1) << " Got: " << symbol.id_ << std::endl;
      exit(1);
    }
    max_id_ = std::max(static_cast<const uint32_t>(max_id_), symbol.id_);
    members_.push_back(symbol);
    member_name_to_id_.insert({symbol.name_, symbol.id_});
    return true;
  }

  /* Query APIs */
  inline uint32_t get_member_id(const std::string &name) const {
    return member_name_to_id_.find(name)->second;
  }

  friend std::ostream &operator<<(std::ostream &os, const Symbol &obj) {
    os << obj.toString();
    return os;
  }
  friend std::ostream &operator<<(std::ostream &os, const Symbol *obj) {
    os << *obj;
    return os;
  }
};

uint32_t Symbol::msg_id_cnt_ = 0;

class SymTab {

public:
  /* Query APIs */
  inline uint32_t getMessageId(const std::string &name) {
    assert(msg_to_id_.find(name) != msg_to_id_.end());
    return msg_to_id_[name];
  }
  inline Symbol *getSymbol(const std::string &name) {
    return getSymbol(getMessageId(name));
  }
  inline Symbol *getSymbol(const uint32_t id) {
    assert(id_to_symbol_.find(id) != id_to_symbol_.end());
    return id_to_symbol_[id];
  }

  /* Access APIs */
  inline static void init() { SymTab::symtab_ = new SymTab(); }
  inline static SymTab *getSymTab() { return SymTab::symtab_; }

  /* Builder APIs */
  bool addSymbol(Symbol *symbol) {
    assert(symbol->message_);
    assert(msg_to_id_.find(symbol->name_) == msg_to_id_.end());
    assert(id_to_msg_.find(symbol->id_) == id_to_msg_.end());
    msg_to_id_.insert({symbol->name_, symbol->id_});
    id_to_msg_.insert({symbol->id_, symbol->name_});
    id_to_symbol_.insert({symbol->id_, symbol});
    return true;
  }

  static void readFile(std::string filePath) {
    std::unordered_map<std::string, ProtoMessage> allMsgs =
        parseProtoFile(filePath);
    for (auto msgPtr : allMsgs) {
      printProtoMessage(msgPtr.second);
      auto msg = msgPtr.second;
      Symbol *m = new Symbol(msg.name);

      getSymTab()->addSymbol(m);
      for (auto field : msg.fields) {
        Symbol sym = Symbol(field.id, field.name, field.isRepeated, m,
                            field.type, false);
        m->addMember(sym);
      }
    }
  }

private:
  SymTab() = default;

  static SymTab *symtab_;
  std::unordered_map<std::string, int32_t> msg_to_id_;
  std::unordered_map<uint32_t, std::string> id_to_msg_;
  std::unordered_map<uint32_t, Symbol *> id_to_symbol_;
};

SymTab *SymTab::symtab_ = nullptr;
} // namespace user
