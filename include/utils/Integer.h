#ifndef UTILS_INTEGER_H
#define UTILS_INTEGER_H

#include "utils/utils.h"
// #include "utils/Log.h"

#include "tlp/traits/TypeTraits.h"
#include "tlp/bool/BoolType.h"

UTILS_NS_BEGIN

static inline size_t AppendInteger(char* buf) { return 0; }
static inline size_t ReadInteger(const char* buf) { return 0; }

template<typename Type1, typename ... Types,
    typename = typename TLP_NS::is_integral_t<Type1>>
static size_t AppendInteger(char* buf, Type1 value, Types ... values) {
  // make sure alignment
  // CHECK_EQ((uintptr_t)buf % sizeof(Type1), Type1(0));
  *(reinterpret_cast<Type1*>(buf)) = value;
  return sizeof(Type1) + AppendInteger(buf + sizeof(Type1), values...);
}

template<typename Type1, typename ... Types,
    typename = typename TLP_NS::is_integral_t<Type1>>
static size_t ReadInteger(const char* buf, Type1& value, Types&... values) {
  // make sure alignment
  // CHECK_EQ((uintptr_t)buf % sizeof(Type1), Type1(0));
  value = *reinterpret_cast<Type1*>(const_cast<char*>(buf));
  return sizeof(Type1)
         + ReadInteger(const_cast<char*>(buf) + sizeof(Type1), values...);
}

template<typename T, int N>
static size_t AppendInteger(char* buf, T* array);

template<typename T, int N>
static size_t AppendInteger(char* buf, T (&array)[N] ) {
  return AppendInteger<T, N>(buf, reinterpret_cast<T*>(array));
}

template<typename T, int N>
static size_t AppendInteger(char* buf, T* array, TLP_NS::BoolType<true>) {
  return AppendInteger<T, 1>(buf, array)
         + AppendInteger<T, N-1>(buf + sizeof(T), array + 1);
}

template<typename T, int N>
static size_t AppendInteger(char* buf, T* array, TLP_NS::BoolType<false>) {
  return AppendInteger(buf, *array);
}

template<typename T, int N>
static size_t AppendInteger(char* buf, T* array) {
  return AppendInteger<T, N>(buf, array, TLP_NS::BoolType<(N>1)>());
}


template<typename T, int N>
static size_t ReadInteger(const char* buf, T* array);

template<typename T, int N>
static size_t ReadInteger(const char* buf, T (&array)[N] ) {
  return ReadInteger<T, N>(buf, reinterpret_cast<T*>(array));
}

template<typename T, int N>
static size_t ReadInteger(const char* buf, T* array, TLP_NS::BoolType<true>) {
  return ReadInteger<T, 1>(buf, array)
         + ReadInteger<T, N-1>(buf + sizeof(T), array + 1);
}

template<typename T, int N>
static size_t ReadInteger(const char* buf, T* array, TLP_NS::BoolType<false>) {
  return ReadInteger(buf, *array);
}

template<typename T, int N>
static size_t ReadInteger(const char* buf, T* array) {
  return ReadInteger<T, N>(buf, array, TLP_NS::BoolType<(N>1)>());
}

UTILS_NS_END
#endif  // UTILS_ENUM_H_
