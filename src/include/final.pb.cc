// Generated by the protocol buffer compiler.  DO NOT EDIT!
// NO CHECKED-IN PROTOBUF GENCODE
// source: final.proto
// Protobuf C++ Version: 5.29.3-dev

#include "final.pb.h"

#include <algorithm>
#include <type_traits>
#include "google/protobuf/io/coded_stream.h"
#include "google/protobuf/generated_message_tctable_impl.h"
#include "google/protobuf/extension_set.h"
#include "google/protobuf/generated_message_util.h"
#include "google/protobuf/wire_format_lite.h"
#include "google/protobuf/descriptor.h"
#include "google/protobuf/generated_message_reflection.h"
#include "google/protobuf/reflection_ops.h"
#include "google/protobuf/wire_format.h"
// @@protoc_insertion_point(includes)

// Must be included last.
#include "google/protobuf/port_def.inc"
PROTOBUF_PRAGMA_INIT_SEG
namespace _pb = ::google::protobuf;
namespace _pbi = ::google::protobuf::internal;
namespace _fl = ::google::protobuf::internal::field_layout;

inline constexpr Person::Impl_::Impl_(
    ::_pbi::ConstantInitialized) noexcept
      : a_(
            &::google::protobuf::internal::fixed_address_empty_string,
            ::_pbi::ConstantInitialized()),
        b_(
            &::google::protobuf::internal::fixed_address_empty_string,
            ::_pbi::ConstantInitialized()),
        c_(
            &::google::protobuf::internal::fixed_address_empty_string,
            ::_pbi::ConstantInitialized()),
        d_(
            &::google::protobuf::internal::fixed_address_empty_string,
            ::_pbi::ConstantInitialized()),
        e_(
            &::google::protobuf::internal::fixed_address_empty_string,
            ::_pbi::ConstantInitialized()),
        f_(
            &::google::protobuf::internal::fixed_address_empty_string,
            ::_pbi::ConstantInitialized()),
        g_(
            &::google::protobuf::internal::fixed_address_empty_string,
            ::_pbi::ConstantInitialized()),
        h_(
            &::google::protobuf::internal::fixed_address_empty_string,
            ::_pbi::ConstantInitialized()),
        i_(
            &::google::protobuf::internal::fixed_address_empty_string,
            ::_pbi::ConstantInitialized()),
        j_(
            &::google::protobuf::internal::fixed_address_empty_string,
            ::_pbi::ConstantInitialized()),
        k_(
            &::google::protobuf::internal::fixed_address_empty_string,
            ::_pbi::ConstantInitialized()),
        _cached_size_{0} {}

template <typename>
PROTOBUF_CONSTEXPR Person::Person(::_pbi::ConstantInitialized)
#if defined(PROTOBUF_CUSTOM_VTABLE)
    : ::google::protobuf::Message(_class_data_.base()),
#else   // PROTOBUF_CUSTOM_VTABLE
    : ::google::protobuf::Message(),
#endif  // PROTOBUF_CUSTOM_VTABLE
      _impl_(::_pbi::ConstantInitialized()) {
}
struct PersonDefaultTypeInternal {
  PROTOBUF_CONSTEXPR PersonDefaultTypeInternal() : _instance(::_pbi::ConstantInitialized{}) {}
  ~PersonDefaultTypeInternal() {}
  union {
    Person _instance;
  };
};

PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT
    PROTOBUF_ATTRIBUTE_INIT_PRIORITY1 PersonDefaultTypeInternal _Person_default_instance_;
static constexpr const ::_pb::EnumDescriptor**
    file_level_enum_descriptors_final_2eproto = nullptr;
static constexpr const ::_pb::ServiceDescriptor**
    file_level_service_descriptors_final_2eproto = nullptr;
const ::uint32_t
    TableStruct_final_2eproto::offsets[] ABSL_ATTRIBUTE_SECTION_VARIABLE(
        protodesc_cold) = {
        ~0u,  // no _has_bits_
        PROTOBUF_FIELD_OFFSET(::Person, _internal_metadata_),
        ~0u,  // no _extensions_
        ~0u,  // no _oneof_case_
        ~0u,  // no _weak_field_map_
        ~0u,  // no _inlined_string_donated_
        ~0u,  // no _split_
        ~0u,  // no sizeof(Split)
        PROTOBUF_FIELD_OFFSET(::Person, _impl_.a_),
        PROTOBUF_FIELD_OFFSET(::Person, _impl_.b_),
        PROTOBUF_FIELD_OFFSET(::Person, _impl_.c_),
        PROTOBUF_FIELD_OFFSET(::Person, _impl_.d_),
        PROTOBUF_FIELD_OFFSET(::Person, _impl_.e_),
        PROTOBUF_FIELD_OFFSET(::Person, _impl_.f_),
        PROTOBUF_FIELD_OFFSET(::Person, _impl_.g_),
        PROTOBUF_FIELD_OFFSET(::Person, _impl_.h_),
        PROTOBUF_FIELD_OFFSET(::Person, _impl_.i_),
        PROTOBUF_FIELD_OFFSET(::Person, _impl_.j_),
        PROTOBUF_FIELD_OFFSET(::Person, _impl_.k_),
};

static const ::_pbi::MigrationSchema
    schemas[] ABSL_ATTRIBUTE_SECTION_VARIABLE(protodesc_cold) = {
        {0, -1, -1, sizeof(::Person)},
};
static const ::_pb::Message* const file_default_instances[] = {
    &::_Person_default_instance_._instance,
};
const char descriptor_table_protodef_final_2eproto[] ABSL_ATTRIBUTE_SECTION_VARIABLE(
    protodesc_cold) = {
    "\n\013final.proto\"\201\001\n\006Person\022\t\n\001A\030\001 \001(\t\022\t\n\001B"
    "\030\002 \001(\t\022\t\n\001C\030\003 \001(\t\022\t\n\001D\030\004 \001(\t\022\t\n\001E\030\005 \001(\t\022"
    "\t\n\001F\030\006 \001(\t\022\t\n\001G\030\007 \001(\t\022\t\n\001H\030\010 \001(\t\022\t\n\001I\030\t "
    "\001(\t\022\t\n\001J\030\n \001(\t\022\t\n\001K\030\013 \001(\tb\006proto3"
};
static ::absl::once_flag descriptor_table_final_2eproto_once;
PROTOBUF_CONSTINIT const ::_pbi::DescriptorTable descriptor_table_final_2eproto = {
    false,
    false,
    153,
    descriptor_table_protodef_final_2eproto,
    "final.proto",
    &descriptor_table_final_2eproto_once,
    nullptr,
    0,
    1,
    schemas,
    file_default_instances,
    TableStruct_final_2eproto::offsets,
    file_level_enum_descriptors_final_2eproto,
    file_level_service_descriptors_final_2eproto,
};
// ===================================================================

class Person::_Internal {
 public:
};

Person::Person(::google::protobuf::Arena* arena)
#if defined(PROTOBUF_CUSTOM_VTABLE)
    : ::google::protobuf::Message(arena, _class_data_.base()) {
#else   // PROTOBUF_CUSTOM_VTABLE
    : ::google::protobuf::Message(arena) {
#endif  // PROTOBUF_CUSTOM_VTABLE
  SharedCtor(arena);
  // @@protoc_insertion_point(arena_constructor:Person)
}
inline PROTOBUF_NDEBUG_INLINE Person::Impl_::Impl_(
    ::google::protobuf::internal::InternalVisibility visibility, ::google::protobuf::Arena* arena,
    const Impl_& from, const ::Person& from_msg)
      : a_(arena, from.a_),
        b_(arena, from.b_),
        c_(arena, from.c_),
        d_(arena, from.d_),
        e_(arena, from.e_),
        f_(arena, from.f_),
        g_(arena, from.g_),
        h_(arena, from.h_),
        i_(arena, from.i_),
        j_(arena, from.j_),
        k_(arena, from.k_),
        _cached_size_{0} {}

Person::Person(
    ::google::protobuf::Arena* arena,
    const Person& from)
#if defined(PROTOBUF_CUSTOM_VTABLE)
    : ::google::protobuf::Message(arena, _class_data_.base()) {
#else   // PROTOBUF_CUSTOM_VTABLE
    : ::google::protobuf::Message(arena) {
#endif  // PROTOBUF_CUSTOM_VTABLE
  Person* const _this = this;
  (void)_this;
  _internal_metadata_.MergeFrom<::google::protobuf::UnknownFieldSet>(
      from._internal_metadata_);
  new (&_impl_) Impl_(internal_visibility(), arena, from._impl_, from);

  // @@protoc_insertion_point(copy_constructor:Person)
}
inline PROTOBUF_NDEBUG_INLINE Person::Impl_::Impl_(
    ::google::protobuf::internal::InternalVisibility visibility,
    ::google::protobuf::Arena* arena)
      : a_(arena),
        b_(arena),
        c_(arena),
        d_(arena),
        e_(arena),
        f_(arena),
        g_(arena),
        h_(arena),
        i_(arena),
        j_(arena),
        k_(arena),
        _cached_size_{0} {}

inline void Person::SharedCtor(::_pb::Arena* arena) {
  new (&_impl_) Impl_(internal_visibility(), arena);
}
Person::~Person() {
  // @@protoc_insertion_point(destructor:Person)
  SharedDtor(*this);
}
inline void Person::SharedDtor(MessageLite& self) {
  Person& this_ = static_cast<Person&>(self);
  this_._internal_metadata_.Delete<::google::protobuf::UnknownFieldSet>();
  ABSL_DCHECK(this_.GetArena() == nullptr);
  this_._impl_.a_.Destroy();
  this_._impl_.b_.Destroy();
  this_._impl_.c_.Destroy();
  this_._impl_.d_.Destroy();
  this_._impl_.e_.Destroy();
  this_._impl_.f_.Destroy();
  this_._impl_.g_.Destroy();
  this_._impl_.h_.Destroy();
  this_._impl_.i_.Destroy();
  this_._impl_.j_.Destroy();
  this_._impl_.k_.Destroy();
  this_._impl_.~Impl_();
}

inline void* Person::PlacementNew_(const void*, void* mem,
                                        ::google::protobuf::Arena* arena) {
  return ::new (mem) Person(arena);
}
constexpr auto Person::InternalNewImpl_() {
  return ::google::protobuf::internal::MessageCreator::CopyInit(sizeof(Person),
                                            alignof(Person));
}
PROTOBUF_CONSTINIT
PROTOBUF_ATTRIBUTE_INIT_PRIORITY1
const ::google::protobuf::internal::ClassDataFull Person::_class_data_ = {
    ::google::protobuf::internal::ClassData{
        &_Person_default_instance_._instance,
        &_table_.header,
        nullptr,  // OnDemandRegisterArenaDtor
        nullptr,  // IsInitialized
        &Person::MergeImpl,
        ::google::protobuf::Message::GetNewImpl<Person>(),
#if defined(PROTOBUF_CUSTOM_VTABLE)
        &Person::SharedDtor,
        ::google::protobuf::Message::GetClearImpl<Person>(), &Person::ByteSizeLong,
            &Person::_InternalSerialize,
#endif  // PROTOBUF_CUSTOM_VTABLE
        PROTOBUF_FIELD_OFFSET(Person, _impl_._cached_size_),
        false,
    },
    &Person::kDescriptorMethods,
    &descriptor_table_final_2eproto,
    nullptr,  // tracker
};
const ::google::protobuf::internal::ClassData* Person::GetClassData() const {
  ::google::protobuf::internal::PrefetchToLocalCache(&_class_data_);
  ::google::protobuf::internal::PrefetchToLocalCache(_class_data_.tc_table);
  return _class_data_.base();
}
PROTOBUF_CONSTINIT PROTOBUF_ATTRIBUTE_INIT_PRIORITY1
const ::_pbi::TcParseTable<4, 11, 0, 34, 2> Person::_table_ = {
  {
    0,  // no _has_bits_
    0, // no _extensions_
    11, 120,  // max_field_number, fast_idx_mask
    offsetof(decltype(_table_), field_lookup_table),
    4294965248,  // skipmap
    offsetof(decltype(_table_), field_entries),
    11,  // num_field_entries
    0,  // num_aux_entries
    offsetof(decltype(_table_), field_names),  // no aux_entries
    _class_data_.base(),
    nullptr,  // post_loop_handler
    ::_pbi::TcParser::GenericFallback,  // fallback
    #ifdef PROTOBUF_PREFETCH_PARSE_TABLE
    ::_pbi::TcParser::GetTable<::Person>(),  // to_prefetch
    #endif  // PROTOBUF_PREFETCH_PARSE_TABLE
  }, {{
    {::_pbi::TcParser::MiniParse, {}},
    // string A = 1;
    {::_pbi::TcParser::FastUS1,
     {10, 63, 0, PROTOBUF_FIELD_OFFSET(Person, _impl_.a_)}},
    // string B = 2;
    {::_pbi::TcParser::FastUS1,
     {18, 63, 0, PROTOBUF_FIELD_OFFSET(Person, _impl_.b_)}},
    // string C = 3;
    {::_pbi::TcParser::FastUS1,
     {26, 63, 0, PROTOBUF_FIELD_OFFSET(Person, _impl_.c_)}},
    // string D = 4;
    {::_pbi::TcParser::FastUS1,
     {34, 63, 0, PROTOBUF_FIELD_OFFSET(Person, _impl_.d_)}},
    // string E = 5;
    {::_pbi::TcParser::FastUS1,
     {42, 63, 0, PROTOBUF_FIELD_OFFSET(Person, _impl_.e_)}},
    // string F = 6;
    {::_pbi::TcParser::FastUS1,
     {50, 63, 0, PROTOBUF_FIELD_OFFSET(Person, _impl_.f_)}},
    // string G = 7;
    {::_pbi::TcParser::FastUS1,
     {58, 63, 0, PROTOBUF_FIELD_OFFSET(Person, _impl_.g_)}},
    // string H = 8;
    {::_pbi::TcParser::FastUS1,
     {66, 63, 0, PROTOBUF_FIELD_OFFSET(Person, _impl_.h_)}},
    // string I = 9;
    {::_pbi::TcParser::FastUS1,
     {74, 63, 0, PROTOBUF_FIELD_OFFSET(Person, _impl_.i_)}},
    // string J = 10;
    {::_pbi::TcParser::FastUS1,
     {82, 63, 0, PROTOBUF_FIELD_OFFSET(Person, _impl_.j_)}},
    // string K = 11;
    {::_pbi::TcParser::FastUS1,
     {90, 63, 0, PROTOBUF_FIELD_OFFSET(Person, _impl_.k_)}},
    {::_pbi::TcParser::MiniParse, {}},
    {::_pbi::TcParser::MiniParse, {}},
    {::_pbi::TcParser::MiniParse, {}},
    {::_pbi::TcParser::MiniParse, {}},
  }}, {{
    65535, 65535
  }}, {{
    // string A = 1;
    {PROTOBUF_FIELD_OFFSET(Person, _impl_.a_), 0, 0,
    (0 | ::_fl::kFcSingular | ::_fl::kUtf8String | ::_fl::kRepAString)},
    // string B = 2;
    {PROTOBUF_FIELD_OFFSET(Person, _impl_.b_), 0, 0,
    (0 | ::_fl::kFcSingular | ::_fl::kUtf8String | ::_fl::kRepAString)},
    // string C = 3;
    {PROTOBUF_FIELD_OFFSET(Person, _impl_.c_), 0, 0,
    (0 | ::_fl::kFcSingular | ::_fl::kUtf8String | ::_fl::kRepAString)},
    // string D = 4;
    {PROTOBUF_FIELD_OFFSET(Person, _impl_.d_), 0, 0,
    (0 | ::_fl::kFcSingular | ::_fl::kUtf8String | ::_fl::kRepAString)},
    // string E = 5;
    {PROTOBUF_FIELD_OFFSET(Person, _impl_.e_), 0, 0,
    (0 | ::_fl::kFcSingular | ::_fl::kUtf8String | ::_fl::kRepAString)},
    // string F = 6;
    {PROTOBUF_FIELD_OFFSET(Person, _impl_.f_), 0, 0,
    (0 | ::_fl::kFcSingular | ::_fl::kUtf8String | ::_fl::kRepAString)},
    // string G = 7;
    {PROTOBUF_FIELD_OFFSET(Person, _impl_.g_), 0, 0,
    (0 | ::_fl::kFcSingular | ::_fl::kUtf8String | ::_fl::kRepAString)},
    // string H = 8;
    {PROTOBUF_FIELD_OFFSET(Person, _impl_.h_), 0, 0,
    (0 | ::_fl::kFcSingular | ::_fl::kUtf8String | ::_fl::kRepAString)},
    // string I = 9;
    {PROTOBUF_FIELD_OFFSET(Person, _impl_.i_), 0, 0,
    (0 | ::_fl::kFcSingular | ::_fl::kUtf8String | ::_fl::kRepAString)},
    // string J = 10;
    {PROTOBUF_FIELD_OFFSET(Person, _impl_.j_), 0, 0,
    (0 | ::_fl::kFcSingular | ::_fl::kUtf8String | ::_fl::kRepAString)},
    // string K = 11;
    {PROTOBUF_FIELD_OFFSET(Person, _impl_.k_), 0, 0,
    (0 | ::_fl::kFcSingular | ::_fl::kUtf8String | ::_fl::kRepAString)},
  }},
  // no aux_entries
  {{
    "\6\1\1\1\1\1\1\1\1\1\1\1\0\0\0\0"
    "Person"
    "A"
    "B"
    "C"
    "D"
    "E"
    "F"
    "G"
    "H"
    "I"
    "J"
    "K"
  }},
};

PROTOBUF_NOINLINE void Person::Clear() {
// @@protoc_insertion_point(message_clear_start:Person)
  ::google::protobuf::internal::TSanWrite(&_impl_);
  ::uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  _impl_.a_.ClearToEmpty();
  _impl_.b_.ClearToEmpty();
  _impl_.c_.ClearToEmpty();
  _impl_.d_.ClearToEmpty();
  _impl_.e_.ClearToEmpty();
  _impl_.f_.ClearToEmpty();
  _impl_.g_.ClearToEmpty();
  _impl_.h_.ClearToEmpty();
  _impl_.i_.ClearToEmpty();
  _impl_.j_.ClearToEmpty();
  _impl_.k_.ClearToEmpty();
  _internal_metadata_.Clear<::google::protobuf::UnknownFieldSet>();
}

#if defined(PROTOBUF_CUSTOM_VTABLE)
        ::uint8_t* Person::_InternalSerialize(
            const MessageLite& base, ::uint8_t* target,
            ::google::protobuf::io::EpsCopyOutputStream* stream) {
          const Person& this_ = static_cast<const Person&>(base);
#else   // PROTOBUF_CUSTOM_VTABLE
        ::uint8_t* Person::_InternalSerialize(
            ::uint8_t* target,
            ::google::protobuf::io::EpsCopyOutputStream* stream) const {
          const Person& this_ = *this;
#endif  // PROTOBUF_CUSTOM_VTABLE
          // @@protoc_insertion_point(serialize_to_array_start:Person)
          ::uint32_t cached_has_bits = 0;
          (void)cached_has_bits;

          // string A = 1;
          if (!this_._internal_a().empty()) {
            const std::string& _s = this_._internal_a();
            ::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
                _s.data(), static_cast<int>(_s.length()), ::google::protobuf::internal::WireFormatLite::SERIALIZE, "Person.A");
            target = stream->WriteStringMaybeAliased(1, _s, target);
          }

          // string B = 2;
          if (!this_._internal_b().empty()) {
            const std::string& _s = this_._internal_b();
            ::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
                _s.data(), static_cast<int>(_s.length()), ::google::protobuf::internal::WireFormatLite::SERIALIZE, "Person.B");
            target = stream->WriteStringMaybeAliased(2, _s, target);
          }

          // string C = 3;
          if (!this_._internal_c().empty()) {
            const std::string& _s = this_._internal_c();
            ::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
                _s.data(), static_cast<int>(_s.length()), ::google::protobuf::internal::WireFormatLite::SERIALIZE, "Person.C");
            target = stream->WriteStringMaybeAliased(3, _s, target);
          }

          // string D = 4;
          if (!this_._internal_d().empty()) {
            const std::string& _s = this_._internal_d();
            ::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
                _s.data(), static_cast<int>(_s.length()), ::google::protobuf::internal::WireFormatLite::SERIALIZE, "Person.D");
            target = stream->WriteStringMaybeAliased(4, _s, target);
          }

          // string E = 5;
          if (!this_._internal_e().empty()) {
            const std::string& _s = this_._internal_e();
            ::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
                _s.data(), static_cast<int>(_s.length()), ::google::protobuf::internal::WireFormatLite::SERIALIZE, "Person.E");
            target = stream->WriteStringMaybeAliased(5, _s, target);
          }

          // string F = 6;
          if (!this_._internal_f().empty()) {
            const std::string& _s = this_._internal_f();
            ::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
                _s.data(), static_cast<int>(_s.length()), ::google::protobuf::internal::WireFormatLite::SERIALIZE, "Person.F");
            target = stream->WriteStringMaybeAliased(6, _s, target);
          }

          // string G = 7;
          if (!this_._internal_g().empty()) {
            const std::string& _s = this_._internal_g();
            ::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
                _s.data(), static_cast<int>(_s.length()), ::google::protobuf::internal::WireFormatLite::SERIALIZE, "Person.G");
            target = stream->WriteStringMaybeAliased(7, _s, target);
          }

          // string H = 8;
          if (!this_._internal_h().empty()) {
            const std::string& _s = this_._internal_h();
            ::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
                _s.data(), static_cast<int>(_s.length()), ::google::protobuf::internal::WireFormatLite::SERIALIZE, "Person.H");
            target = stream->WriteStringMaybeAliased(8, _s, target);
          }

          // string I = 9;
          if (!this_._internal_i().empty()) {
            const std::string& _s = this_._internal_i();
            ::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
                _s.data(), static_cast<int>(_s.length()), ::google::protobuf::internal::WireFormatLite::SERIALIZE, "Person.I");
            target = stream->WriteStringMaybeAliased(9, _s, target);
          }

          // string J = 10;
          if (!this_._internal_j().empty()) {
            const std::string& _s = this_._internal_j();
            ::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
                _s.data(), static_cast<int>(_s.length()), ::google::protobuf::internal::WireFormatLite::SERIALIZE, "Person.J");
            target = stream->WriteStringMaybeAliased(10, _s, target);
          }

          // string K = 11;
          if (!this_._internal_k().empty()) {
            const std::string& _s = this_._internal_k();
            ::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
                _s.data(), static_cast<int>(_s.length()), ::google::protobuf::internal::WireFormatLite::SERIALIZE, "Person.K");
            target = stream->WriteStringMaybeAliased(11, _s, target);
          }

          if (PROTOBUF_PREDICT_FALSE(this_._internal_metadata_.have_unknown_fields())) {
            target =
                ::_pbi::WireFormat::InternalSerializeUnknownFieldsToArray(
                    this_._internal_metadata_.unknown_fields<::google::protobuf::UnknownFieldSet>(::google::protobuf::UnknownFieldSet::default_instance), target, stream);
          }
          // @@protoc_insertion_point(serialize_to_array_end:Person)
          return target;
        }

#if defined(PROTOBUF_CUSTOM_VTABLE)
        ::size_t Person::ByteSizeLong(const MessageLite& base) {
          const Person& this_ = static_cast<const Person&>(base);
#else   // PROTOBUF_CUSTOM_VTABLE
        ::size_t Person::ByteSizeLong() const {
          const Person& this_ = *this;
#endif  // PROTOBUF_CUSTOM_VTABLE
          // @@protoc_insertion_point(message_byte_size_start:Person)
          ::size_t total_size = 0;

          ::uint32_t cached_has_bits = 0;
          // Prevent compiler warnings about cached_has_bits being unused
          (void)cached_has_bits;

          ::_pbi::Prefetch5LinesFrom7Lines(&this_);
           {
            // string A = 1;
            if (!this_._internal_a().empty()) {
              total_size += 1 + ::google::protobuf::internal::WireFormatLite::StringSize(
                                              this_._internal_a());
            }
            // string B = 2;
            if (!this_._internal_b().empty()) {
              total_size += 1 + ::google::protobuf::internal::WireFormatLite::StringSize(
                                              this_._internal_b());
            }
            // string C = 3;
            if (!this_._internal_c().empty()) {
              total_size += 1 + ::google::protobuf::internal::WireFormatLite::StringSize(
                                              this_._internal_c());
            }
            // string D = 4;
            if (!this_._internal_d().empty()) {
              total_size += 1 + ::google::protobuf::internal::WireFormatLite::StringSize(
                                              this_._internal_d());
            }
            // string E = 5;
            if (!this_._internal_e().empty()) {
              total_size += 1 + ::google::protobuf::internal::WireFormatLite::StringSize(
                                              this_._internal_e());
            }
            // string F = 6;
            if (!this_._internal_f().empty()) {
              total_size += 1 + ::google::protobuf::internal::WireFormatLite::StringSize(
                                              this_._internal_f());
            }
            // string G = 7;
            if (!this_._internal_g().empty()) {
              total_size += 1 + ::google::protobuf::internal::WireFormatLite::StringSize(
                                              this_._internal_g());
            }
            // string H = 8;
            if (!this_._internal_h().empty()) {
              total_size += 1 + ::google::protobuf::internal::WireFormatLite::StringSize(
                                              this_._internal_h());
            }
            // string I = 9;
            if (!this_._internal_i().empty()) {
              total_size += 1 + ::google::protobuf::internal::WireFormatLite::StringSize(
                                              this_._internal_i());
            }
            // string J = 10;
            if (!this_._internal_j().empty()) {
              total_size += 1 + ::google::protobuf::internal::WireFormatLite::StringSize(
                                              this_._internal_j());
            }
            // string K = 11;
            if (!this_._internal_k().empty()) {
              total_size += 1 + ::google::protobuf::internal::WireFormatLite::StringSize(
                                              this_._internal_k());
            }
          }
          return this_.MaybeComputeUnknownFieldsSize(total_size,
                                                     &this_._impl_._cached_size_);
        }

void Person::MergeImpl(::google::protobuf::MessageLite& to_msg, const ::google::protobuf::MessageLite& from_msg) {
  auto* const _this = static_cast<Person*>(&to_msg);
  auto& from = static_cast<const Person&>(from_msg);
  // @@protoc_insertion_point(class_specific_merge_from_start:Person)
  ABSL_DCHECK_NE(&from, _this);
  ::uint32_t cached_has_bits = 0;
  (void) cached_has_bits;

  if (!from._internal_a().empty()) {
    _this->_internal_set_a(from._internal_a());
  }
  if (!from._internal_b().empty()) {
    _this->_internal_set_b(from._internal_b());
  }
  if (!from._internal_c().empty()) {
    _this->_internal_set_c(from._internal_c());
  }
  if (!from._internal_d().empty()) {
    _this->_internal_set_d(from._internal_d());
  }
  if (!from._internal_e().empty()) {
    _this->_internal_set_e(from._internal_e());
  }
  if (!from._internal_f().empty()) {
    _this->_internal_set_f(from._internal_f());
  }
  if (!from._internal_g().empty()) {
    _this->_internal_set_g(from._internal_g());
  }
  if (!from._internal_h().empty()) {
    _this->_internal_set_h(from._internal_h());
  }
  if (!from._internal_i().empty()) {
    _this->_internal_set_i(from._internal_i());
  }
  if (!from._internal_j().empty()) {
    _this->_internal_set_j(from._internal_j());
  }
  if (!from._internal_k().empty()) {
    _this->_internal_set_k(from._internal_k());
  }
  _this->_internal_metadata_.MergeFrom<::google::protobuf::UnknownFieldSet>(from._internal_metadata_);
}

void Person::CopyFrom(const Person& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:Person)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}


void Person::InternalSwap(Person* PROTOBUF_RESTRICT other) {
  using std::swap;
  auto* arena = GetArena();
  ABSL_DCHECK_EQ(arena, other->GetArena());
  _internal_metadata_.InternalSwap(&other->_internal_metadata_);
  ::_pbi::ArenaStringPtr::InternalSwap(&_impl_.a_, &other->_impl_.a_, arena);
  ::_pbi::ArenaStringPtr::InternalSwap(&_impl_.b_, &other->_impl_.b_, arena);
  ::_pbi::ArenaStringPtr::InternalSwap(&_impl_.c_, &other->_impl_.c_, arena);
  ::_pbi::ArenaStringPtr::InternalSwap(&_impl_.d_, &other->_impl_.d_, arena);
  ::_pbi::ArenaStringPtr::InternalSwap(&_impl_.e_, &other->_impl_.e_, arena);
  ::_pbi::ArenaStringPtr::InternalSwap(&_impl_.f_, &other->_impl_.f_, arena);
  ::_pbi::ArenaStringPtr::InternalSwap(&_impl_.g_, &other->_impl_.g_, arena);
  ::_pbi::ArenaStringPtr::InternalSwap(&_impl_.h_, &other->_impl_.h_, arena);
  ::_pbi::ArenaStringPtr::InternalSwap(&_impl_.i_, &other->_impl_.i_, arena);
  ::_pbi::ArenaStringPtr::InternalSwap(&_impl_.j_, &other->_impl_.j_, arena);
  ::_pbi::ArenaStringPtr::InternalSwap(&_impl_.k_, &other->_impl_.k_, arena);
}

::google::protobuf::Metadata Person::GetMetadata() const {
  return ::google::protobuf::Message::GetMetadataImpl(GetClassData()->full());
}
// @@protoc_insertion_point(namespace_scope)
namespace google {
namespace protobuf {
}  // namespace protobuf
}  // namespace google
// @@protoc_insertion_point(global_scope)
PROTOBUF_ATTRIBUTE_INIT_PRIORITY2 static ::std::false_type
    _static_init2_ PROTOBUF_UNUSED =
        (::_pbi::AddDescriptors(&descriptor_table_final_2eproto),
         ::std::false_type{});
#include "google/protobuf/port_undef.inc"
