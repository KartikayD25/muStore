#pragma once

#include <exception>
#include <optional>
#include <typeinfo>

#include "define.hpp"
#include "net/common.hpp"
#include "symtab.hpp"
#include "types.hpp"

namespace user {
class DesrialiserMetaBase {
public:
  virtual void dummy() { NOT_IMPLEMENTED; }
};

/*
 * Primitive type and non-repeated
 */
template <typename T> class PrimitiveSingleMeta : public DesrialiserMetaBase {
public:
  PrimitiveSingleMeta() = delete;
  PrimitiveSingleMeta(T val) : val_(val) {}
  auto get() const -> T { return val_; }

private:
  const T val_;
};

/*
 * Primitive type and repeated
 */
class PrimitiveRepeatedMeta : public DesrialiserMetaBase {
public:
  PrimitiveRepeatedMeta() = delete;
  PrimitiveRepeatedMeta(const char *base, const uint32_t size,
                        const uint32_t item_size)
      : item_size_(item_size), size_(size), list_base_(base) {}

  template <typename T> auto get(uint32_t idx) -> std::optional<const T> {
    if (idx >= size_) {
      return std::nullopt;
    }
    return *reinterpret_cast<const T *>(list_base_ + idx * item_size_);
  }
  auto size() const -> const uint32_t { return size_; }

private:
  const uint32_t item_size_{0};
  const uint32_t size_{0};
  const char *list_base_{nullptr};
};

/*
 * Complex type and non-repeated
 */
class ByteSingleMeta : public DesrialiserMetaBase {
public:
  ByteSingleMeta() = delete;
  ByteSingleMeta(const char *base, const uint32_t len)
      : base_(base), len_(len) {}
  auto get() -> byte_t { return std::make_pair(base_, len_); }
  auto getString() -> std::string { return std::string(base_, base_ + len_); }
  auto assignString(std::string &str) -> void { str.assign(base_, len_); }

private:
  const char *base_{nullptr};
  const uint32_t len_{0};
};

/*
 * Complex type and repeated
 */
class ByteRepeatedMeta : public DesrialiserMetaBase {
public:
  ByteRepeatedMeta() = delete;
  ByteRepeatedMeta(const char *base, const uint32_t len)
      : base_(base), len_(len) {
    items_.resize(len_);
  }
  auto get(uint32_t idx) -> std::optional<byte_t> {
    if (idx >= len_) {
      return std::nullopt;
    }
    offloadHeaders(idx);
    return items_[idx];
  }
  auto size() const -> const uint32_t { return len_; }

  auto offloadHeaders(uint32_t idx) -> void {
    // for (; offloaded_ <= idx; offloaded_++) {
    while (offloaded_ <= idx) {
      auto item_len =
          *reinterpret_cast<const uint32_t *>(base_ + offload_offset_);
      items_[offloaded_] =
          std::make_pair(base_ + offload_offset_ + 4, item_len);
      offload_offset_ += 4 + item_len;
      offloaded_++;
    }
  }

private:
  const char *base_{nullptr};
  const uint32_t len_{0};
  uint32_t offloaded_{0}, offload_offset_{0};
  std::vector<byte_t> items_;
};
} // namespace user
