#pragma once

#include "define.hpp"

// namespace user {

// enum class UserTypes : uint8_t {
//   TYPE_UNKNOWN,
//   TYPE_BOOL,
//   TYPE_INT32,
//   TYPE_INT64,
//   TYPE_UINT32,
//   TYPE_UINT64,
//   TYPE_FLOAT,
//   TYPE_DOUBLE,
//   TYPE_STRING,
//   TYPE_BYTES,
//   TYPE_MESSAGE
// };
// namespace TypeHelper {

// static std::string typeToStr(UserTypes type) {
//   switch (type) {
//   case UserTypes::TYPE_UNKNOWN:
//     return "unknown";
//   case UserTypes::TYPE_BOOL:
//     return "bool";
//   case UserTypes::TYPE_INT32:
//     return "int32";
//   case UserTypes::TYPE_INT64:
//     return "int64";
//   case UserTypes::TYPE_UINT32:
//     return "uint32";
//   case UserTypes::TYPE_UINT64:
//     return "uint64";
//   case UserTypes::TYPE_FLOAT:
//     return "float";
//   case UserTypes::TYPE_DOUBLE:
//     return "double";
//   case UserTypes::TYPE_STRING:
//     return "string";
//   case UserTypes::TYPE_BYTES:
//     return "bytes";
//   case UserTypes::TYPE_MESSAGE:
//     return "message";
//   }
//   std::cerr << LINE_LOCATION << "type string not found. type: " << ((int)type)
//             << std::endl;
//   exit(1);
// }
// static inline bool isComplexType(UserTypes type) {
//   return type == UserTypes::TYPE_STRING || type == UserTypes::TYPE_BYTES;
// }

// // Size of uSer types in bytes
// static uint32_t typeSize(UserTypes type) {
//   switch (type) {
//   case UserTypes::TYPE_BOOL:
//     return 1;
//   case UserTypes::TYPE_INT32:
//     return 4;
//   case UserTypes::TYPE_INT64:
//     return 8;
//   case UserTypes::TYPE_UINT32:
//     return 4;
//   case UserTypes::TYPE_UINT64:
//     return 8;
//   case UserTypes::TYPE_FLOAT:
//     return 4;
//   case UserTypes::TYPE_DOUBLE:
//     return 8;
//   case UserTypes::TYPE_UNKNOWN:
//   case UserTypes::TYPE_STRING:
//   case UserTypes::TYPE_BYTES:
//   case UserTypes::TYPE_MESSAGE:
//     break;
//   };
//   std::cerr << LINE_LOCATION << "Size not defined for type. " << typeToStr(type)
//             << std::endl;
//   exit(1);
// }

// template <typename T>
// // static bool isSamePrimitveType(UserTypes type, T cpp_type) {
// static bool isSamePrimitveType(UserTypes type) {
//   switch (type) {
//   case UserTypes::TYPE_BOOL:
//     return std::is_same<T, bool>::value;
//   case UserTypes::TYPE_INT32:
//     return std::is_same<T, int32_t>::value;
//   case UserTypes::TYPE_INT64:
//     return std::is_same<T, int64_t>::value;
//   case UserTypes::TYPE_UINT32:
//     return std::is_same<T, uint32_t>::value;
//   case UserTypes::TYPE_UINT64:
//     return std::is_same<T, uint64_t>::value;
//   case UserTypes::TYPE_FLOAT:
//     return std::is_same<T, float>::value;
//   case UserTypes::TYPE_DOUBLE:
//     return std::is_same<T, double>::value;
//   };
//   return false;
// }

// user::UserTypes stringToEnum(std::string strType) {
//   if (strType == "bool")
//     return UserTypes::TYPE_BOOL;
//   if (strType == "int32")
//     return UserTypes::TYPE_INT32;
//   if (strType == "int64")
//     return UserTypes::TYPE_INT64;
//   if (strType == "uint32")
//     return UserTypes::TYPE_UINT32;
//   if (strType == "uint64")
//     return UserTypes::TYPE_UINT64;
//   if (strType == "float")
//     return UserTypes::TYPE_FLOAT;
//   if (strType == "double")
//     return UserTypes::TYPE_DOUBLE;
//   if (strType == "string")
//     return UserTypes::TYPE_STRING;
//   if (strType == "bytes")
//     return UserTypes::TYPE_BYTES;
//   if (strType == "message")
//     return UserTypes::TYPE_MESSAGE;
//   else
//     return UserTypes::TYPE_UNKNOWN;
// }

// } // namespace TypeHelper
// /*
// class Conversion {
//   template <typename T> T toCPPvalPtr(gen_ptr_t val, UserTypes type) {
//     switch (type) {
//     case UserTypes::TYPE_BOOL:
//       return static_cast<bool *>(val);
//     case UserTypes::TYPE_INT32:
//       return static_cast<int32_t *>(val);
//     case UserTypes::TYPE_INT64:
//       return static_cast<int64_t *>(val);
//     case UserTypes::TYPE_UINT32:
//       return static_cast<uint32_t *>(val);
//     case UserTypes::TYPE_UINT64:
//       return static_cast<uint64_t *>(val);
//     case UserTypes::TYPE_FLOAT:
//       return static_cast<float *>(val);
//     default:
//       std::cout << LINE_LOCATION
//                 << "Type conversion not implemented. to: " << type << std::endl;
//       exit(1);
//     }
//     return nullptr;
//   }
//   */
// } // namespace user



#pragma once
#include "define.hpp"
#include <type_traits>
#include <string>
#include <iostream>

namespace user {

enum class UserTypes : uint8_t {
    TYPE_UNKNOWN,
    TYPE_BOOL,
    TYPE_INT32,
    TYPE_INT64,
    TYPE_UINT32,
    TYPE_UINT64,
    TYPE_FLOAT,
    TYPE_DOUBLE,
    TYPE_STRING,
    TYPE_BYTES,
    TYPE_MESSAGE
};

namespace TypeHelper {

// Existing runtime functions
static std::string typeToStr(UserTypes type) {
    switch (type) {
        case UserTypes::TYPE_UNKNOWN: return "unknown";
        case UserTypes::TYPE_BOOL: return "bool";
        case UserTypes::TYPE_INT32: return "int32";
        case UserTypes::TYPE_INT64: return "int64";
        case UserTypes::TYPE_UINT32: return "uint32";
        case UserTypes::TYPE_UINT64: return "uint64";
        case UserTypes::TYPE_FLOAT: return "float";
        case UserTypes::TYPE_DOUBLE: return "double";
        case UserTypes::TYPE_STRING: return "string";
        case UserTypes::TYPE_BYTES: return "bytes";
        case UserTypes::TYPE_MESSAGE: return "message";
    }
    std::cerr << LINE_LOCATION << "type string not found. type: " << ((int)type) << std::endl;
    exit(1);
}

static inline bool isComplexType(UserTypes type) {
    return type == UserTypes::TYPE_STRING || type == UserTypes::TYPE_BYTES;
}

static uint32_t typeSize(UserTypes type) {
    switch (type) {
        case UserTypes::TYPE_BOOL: return 1;
        case UserTypes::TYPE_INT32: return 4;
        case UserTypes::TYPE_INT64: return 8;
        case UserTypes::TYPE_UINT32: return 4;
        case UserTypes::TYPE_UINT64: return 8;
        case UserTypes::TYPE_FLOAT: return 4;
        case UserTypes::TYPE_DOUBLE: return 8;
        case UserTypes::TYPE_UNKNOWN:
        case UserTypes::TYPE_STRING:
        case UserTypes::TYPE_BYTES:
        case UserTypes::TYPE_MESSAGE:
            break;
    };
    std::cerr << LINE_LOCATION << "Size not defined for type. " << typeToStr(type) << std::endl;
    exit(1);
}

// New compile-time type traits
template<typename T>
struct TypeTraits;

// Specializations for each type
template<> struct TypeTraits<bool> {
    static constexpr UserTypes type = UserTypes::TYPE_BOOL;
    static constexpr bool is_complex = false;
    static constexpr uint32_t size = 1;
};

template<> struct TypeTraits<int32_t> {
    static constexpr UserTypes type = UserTypes::TYPE_INT32;
    static constexpr bool is_complex = false;
    static constexpr uint32_t size = 4;
};

template<> struct TypeTraits<int64_t> {
    static constexpr UserTypes type = UserTypes::TYPE_INT64;
    static constexpr bool is_complex = false;
    static constexpr uint32_t size = 8;
};

template<> struct TypeTraits<uint32_t> {
    static constexpr UserTypes type = UserTypes::TYPE_UINT32;
    static constexpr bool is_complex = false;
    static constexpr uint32_t size = 4;
};

template<> struct TypeTraits<uint64_t> {
    static constexpr UserTypes type = UserTypes::TYPE_UINT64;
    static constexpr bool is_complex = false;
    static constexpr uint32_t size = 8;
};

template<> struct TypeTraits<float> {
    static constexpr UserTypes type = UserTypes::TYPE_FLOAT;
    static constexpr bool is_complex = false;
    static constexpr uint32_t size = 4;
};

template<> struct TypeTraits<double> {
    static constexpr UserTypes type = UserTypes::TYPE_DOUBLE;
    static constexpr bool is_complex = false;
    static constexpr uint32_t size = 8;
};

template<> struct TypeTraits<std::string> {
    static constexpr UserTypes type = UserTypes::TYPE_STRING;
    static constexpr bool is_complex = true;
    static constexpr uint32_t size = 0; // Variable size
};

// Compile-time type checking functions
template<typename T>
struct IsComplexType {
    static constexpr bool value = TypeTraits<T>::is_complex;
};

template<typename T>
struct GetUserType {
    static constexpr UserTypes value = TypeTraits<T>::type;
};

template<typename T>
struct TypeSize {
    static constexpr uint32_t value = TypeTraits<T>::size;
};

// Compile-time type comparison
template<typename T>
constexpr bool isSameType(UserTypes type) {
    return TypeTraits<T>::type == type;
}

// Helper for checking if a type is a valid user type
// template<typename T>
// struct IsValidUserType {
//     static constexpr bool value = std::is_same_v<T, bool> ||
//                                  std::is_same_v<T, int32_t> ||
//                                  std::is_same_v<T, int64_t> ||
//                                  std::is_same_v<T, uint32_t> ||
//                                  std::is_same_v<T, uint64_t> ||
//                                  std::is_same_v<T, float> ||
//                                  std::is_same_v<T, double> ||
//                                  std::is_same_v<T, std::string>;
// };

// Existing runtime conversion function
static UserTypes stringToEnum(std::string strType) {
    if (strType == "bool") return UserTypes::TYPE_BOOL;
    if (strType == "int32") return UserTypes::TYPE_INT32;
    if (strType == "int64") return UserTypes::TYPE_INT64;
    if (strType == "uint32") return UserTypes::TYPE_UINT32;
    if (strType == "uint64") return UserTypes::TYPE_UINT64;
    if (strType == "float") return UserTypes::TYPE_FLOAT;
    if (strType == "double") return UserTypes::TYPE_DOUBLE;
    if (strType == "string") return UserTypes::TYPE_STRING;
    if (strType == "bytes") return UserTypes::TYPE_BYTES;
    if (strType == "message") return UserTypes::TYPE_MESSAGE;
    return UserTypes::TYPE_UNKNOWN;
}

} // namespace TypeHelper
} // namespace user


