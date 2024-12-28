// automatically generated by the FlatBuffers compiler, do not modify


#ifndef FLATBUFFERS_GENERATED_FINAL_USER_TEST_PROTO_H_
#define FLATBUFFERS_GENERATED_FINAL_USER_TEST_PROTO_H_

#include "flatbuffers/flatbuffers.h"

// Ensure the included flatbuffers.h is the same version as when this file was
// generated, otherwise it may not be compatible.
static_assert(FLATBUFFERS_VERSION_MAJOR == 24 &&
              FLATBUFFERS_VERSION_MINOR == 12 &&
              FLATBUFFERS_VERSION_REVISION == 23,
             "Non-compatible flatbuffers version included");

namespace uSer {
namespace test {
namespace proto {

struct Person;
struct PersonBuilder;

struct Person FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
  typedef PersonBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_A = 4,
    VT_B = 6,
    VT_D = 8,
    VT_E = 10,
    VT_F = 12,
    VT_G = 14,
    VT_H = 16,
    VT_I = 18,
    VT_J = 20,
    VT_K = 22,
    VT_L = 24
  };
  const ::flatbuffers::String *a() const {
    return GetPointer<const ::flatbuffers::String *>(VT_A);
  }
  const ::flatbuffers::String *b() const {
    return GetPointer<const ::flatbuffers::String *>(VT_B);
  }
  const ::flatbuffers::String *d() const {
    return GetPointer<const ::flatbuffers::String *>(VT_D);
  }
  const ::flatbuffers::String *e() const {
    return GetPointer<const ::flatbuffers::String *>(VT_E);
  }
  const ::flatbuffers::String *f() const {
    return GetPointer<const ::flatbuffers::String *>(VT_F);
  }
  const ::flatbuffers::String *g() const {
    return GetPointer<const ::flatbuffers::String *>(VT_G);
  }
  const ::flatbuffers::String *h() const {
    return GetPointer<const ::flatbuffers::String *>(VT_H);
  }
  const ::flatbuffers::String *i() const {
    return GetPointer<const ::flatbuffers::String *>(VT_I);
  }
  const ::flatbuffers::String *j() const {
    return GetPointer<const ::flatbuffers::String *>(VT_J);
  }
  const ::flatbuffers::String *k() const {
    return GetPointer<const ::flatbuffers::String *>(VT_K);
  }
  const ::flatbuffers::String *l() const {
    return GetPointer<const ::flatbuffers::String *>(VT_L);
  }
  bool Verify(::flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyOffset(verifier, VT_A) &&
           verifier.VerifyString(a()) &&
           VerifyOffset(verifier, VT_B) &&
           verifier.VerifyString(b()) &&
           VerifyOffset(verifier, VT_D) &&
           verifier.VerifyString(d()) &&
           VerifyOffset(verifier, VT_E) &&
           verifier.VerifyString(e()) &&
           VerifyOffset(verifier, VT_F) &&
           verifier.VerifyString(f()) &&
           VerifyOffset(verifier, VT_G) &&
           verifier.VerifyString(g()) &&
           VerifyOffset(verifier, VT_H) &&
           verifier.VerifyString(h()) &&
           VerifyOffset(verifier, VT_I) &&
           verifier.VerifyString(i()) &&
           VerifyOffset(verifier, VT_J) &&
           verifier.VerifyString(j()) &&
           VerifyOffset(verifier, VT_K) &&
           verifier.VerifyString(k()) &&
           VerifyOffset(verifier, VT_L) &&
           verifier.VerifyString(l()) &&
           verifier.EndTable();
  }
};

struct PersonBuilder {
  typedef Person Table;
  ::flatbuffers::FlatBufferBuilder &fbb_;
  ::flatbuffers::uoffset_t start_;
  void add_a(::flatbuffers::Offset<::flatbuffers::String> a) {
    fbb_.AddOffset(Person::VT_A, a);
  }
  void add_b(::flatbuffers::Offset<::flatbuffers::String> b) {
    fbb_.AddOffset(Person::VT_B, b);
  }
  void add_d(::flatbuffers::Offset<::flatbuffers::String> d) {
    fbb_.AddOffset(Person::VT_D, d);
  }
  void add_e(::flatbuffers::Offset<::flatbuffers::String> e) {
    fbb_.AddOffset(Person::VT_E, e);
  }
  void add_f(::flatbuffers::Offset<::flatbuffers::String> f) {
    fbb_.AddOffset(Person::VT_F, f);
  }
  void add_g(::flatbuffers::Offset<::flatbuffers::String> g) {
    fbb_.AddOffset(Person::VT_G, g);
  }
  void add_h(::flatbuffers::Offset<::flatbuffers::String> h) {
    fbb_.AddOffset(Person::VT_H, h);
  }
  void add_i(::flatbuffers::Offset<::flatbuffers::String> i) {
    fbb_.AddOffset(Person::VT_I, i);
  }
  void add_j(::flatbuffers::Offset<::flatbuffers::String> j) {
    fbb_.AddOffset(Person::VT_J, j);
  }
  void add_k(::flatbuffers::Offset<::flatbuffers::String> k) {
    fbb_.AddOffset(Person::VT_K, k);
  }
  void add_l(::flatbuffers::Offset<::flatbuffers::String> l) {
    fbb_.AddOffset(Person::VT_L, l);
  }
  explicit PersonBuilder(::flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  ::flatbuffers::Offset<Person> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = ::flatbuffers::Offset<Person>(end);
    return o;
  }
};

inline ::flatbuffers::Offset<Person> CreatePerson(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    ::flatbuffers::Offset<::flatbuffers::String> a = 0,
    ::flatbuffers::Offset<::flatbuffers::String> b = 0,
    ::flatbuffers::Offset<::flatbuffers::String> d = 0,
    ::flatbuffers::Offset<::flatbuffers::String> e = 0,
    ::flatbuffers::Offset<::flatbuffers::String> f = 0,
    ::flatbuffers::Offset<::flatbuffers::String> g = 0,
    ::flatbuffers::Offset<::flatbuffers::String> h = 0,
    ::flatbuffers::Offset<::flatbuffers::String> i = 0,
    ::flatbuffers::Offset<::flatbuffers::String> j = 0,
    ::flatbuffers::Offset<::flatbuffers::String> k = 0,
    ::flatbuffers::Offset<::flatbuffers::String> l = 0) {
  PersonBuilder builder_(_fbb);
  builder_.add_l(l);
  builder_.add_k(k);
  builder_.add_j(j);
  builder_.add_i(i);
  builder_.add_h(h);
  builder_.add_g(g);
  builder_.add_f(f);
  builder_.add_e(e);
  builder_.add_d(d);
  builder_.add_b(b);
  builder_.add_a(a);
  return builder_.Finish();
}

inline ::flatbuffers::Offset<Person> CreatePersonDirect(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    const char *a = nullptr,
    const char *b = nullptr,
    const char *d = nullptr,
    const char *e = nullptr,
    const char *f = nullptr,
    const char *g = nullptr,
    const char *h = nullptr,
    const char *i = nullptr,
    const char *j = nullptr,
    const char *k = nullptr,
    const char *l = nullptr) {
  auto a__ = a ? _fbb.CreateString(a) : 0;
  auto b__ = b ? _fbb.CreateString(b) : 0;
  auto d__ = d ? _fbb.CreateString(d) : 0;
  auto e__ = e ? _fbb.CreateString(e) : 0;
  auto f__ = f ? _fbb.CreateString(f) : 0;
  auto g__ = g ? _fbb.CreateString(g) : 0;
  auto h__ = h ? _fbb.CreateString(h) : 0;
  auto i__ = i ? _fbb.CreateString(i) : 0;
  auto j__ = j ? _fbb.CreateString(j) : 0;
  auto k__ = k ? _fbb.CreateString(k) : 0;
  auto l__ = l ? _fbb.CreateString(l) : 0;
  return uSer::test::proto::CreatePerson(
      _fbb,
      a__,
      b__,
      d__,
      e__,
      f__,
      g__,
      h__,
      i__,
      j__,
      k__,
      l__);
}

inline const uSer::test::proto::Person *GetPerson(const void *buf) {
  return ::flatbuffers::GetRoot<uSer::test::proto::Person>(buf);
}

inline const uSer::test::proto::Person *GetSizePrefixedPerson(const void *buf) {
  return ::flatbuffers::GetSizePrefixedRoot<uSer::test::proto::Person>(buf);
}

inline bool VerifyPersonBuffer(
    ::flatbuffers::Verifier &verifier) {
  return verifier.VerifyBuffer<uSer::test::proto::Person>(nullptr);
}

inline bool VerifySizePrefixedPersonBuffer(
    ::flatbuffers::Verifier &verifier) {
  return verifier.VerifySizePrefixedBuffer<uSer::test::proto::Person>(nullptr);
}

inline void FinishPersonBuffer(
    ::flatbuffers::FlatBufferBuilder &fbb,
    ::flatbuffers::Offset<uSer::test::proto::Person> root) {
  fbb.Finish(root);
}

inline void FinishSizePrefixedPersonBuffer(
    ::flatbuffers::FlatBufferBuilder &fbb,
    ::flatbuffers::Offset<uSer::test::proto::Person> root) {
  fbb.FinishSizePrefixed(root);
}

}  // namespace proto
}  // namespace test
}  // namespace uSer

#endif  // FLATBUFFERS_GENERATED_FINAL_USER_TEST_PROTO_H_