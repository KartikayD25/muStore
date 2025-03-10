// Need to create a tool to generate build time code ready to be used at compile time


// 1. What all code is needed to be generated? 
// 2. Do we generate seperate message class code specific to each message struct in the protofile? 




// Enter a proto file --> Proto file needs to be parsed, Symbol table needs to be created => Can be done using symtab.hpp file.
// Do we create a symbol table at build time?? 
// Can be done by doing symtab::init and then read the proto file using its parsing method and filling the symtab
// Then generate the Message class, give the class name of the struct?? Actually yes.
// Constructor: //replace message_name var with the actual message name.
// constructor ensures that the message_name_ and symbol_ class?instance variables are known 
// If the Message class is generated unique to each message struct, then constructor just performs init(), message_name_ and symbol_ class variables will be set at build time itself.
//   Message(std::string &message_name)
//       : message_name_(message_name), symbol_(SYMTAB->getSymbol(message_name_)) {
//     init();
//   }
// init() generator:  symbol_ set at build time? Import FieldValue,TypeHelper wala code file?
//   void init() {
//     for (const auto &mem_sym : symbol_->members_) {
//       FieldValue *val = nullptr;
//       if (TypeHelper::isComplexType(mem_sym.type_)) { //TypeHelper is known at build time as is the type(symtab).
//         if (mem_sym.repeated_) {
//           val = new RepeatedComplexFieldValue(mem_sym);
//         } else {
//           val = new SingleComplexFieldValue(mem_sym);
//         }
//       } else {
//         if (mem_sym.repeated_) {
//           val = new RepeatedSimpleFieldValue(mem_sym);
//         } else {
//           val = new SingleSimpleFieldValue(mem_sym);
//         }
//       }
//       values_.push_back(val);
//     }
//   }
// Does the FieldValue and its children classes' code need to be generated specific to each message type?? I think yes. Only then it makes sense. 
/* For init() to work properly, FieldValue and its children classes need to be generated first.
   Generate the private fields according to the message.
*/


/*  values_, symbol_, message_name class variables need to be filled. 
    need to generate for each field, setters and getters, owned by that message type. 
    addval types will be generated for repeated types. 
    
    example setVal for reference:
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

  For each field, type is known at build time, no need to cast it. get_member_id can be made to be a static function. if it isnt already.
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






//// NEED TO HAVE ALL THE VECTORS WITH 32 INDICES, AND MAINTAIN A COUNTER IN THE init() FUNCTION, TO FILL THE RESPECTIVE IDS ONLY.










*/









