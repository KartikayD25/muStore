#pragma once

#include <cstdint>
#include <string>

#define LINE_LOCATION                                                          \
  __FILE__ << ":" << __LINE__ << "::" << __FUNCTION__ << "\t"

#define SYMTAB SymTab::getSymTab()

namespace user {

typedef void const *gen_ptr_t;
typedef std::pair<const char *, uint32_t> byte_t;

enum class SerializationMethod { COPY, SG, ZC};

std::string SerializationMethodToStr(SerializationMethod method) {
  return method == SerializationMethod::COPY ? "COPY" : "SG";
}
} // namespace user
