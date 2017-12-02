#ifndef TLP_TRAITS_TYPE_TRAITS_H
#define TLP_TRAITS_TYPE_TRAITS_H

#include <type_traits>

#include "tlp/tlp.h"
#include "tlp/traits/IsBaseOf.h"

TLP_NS_BEGIN

template <typename T>
struct is_char{
    constexpr static bool Value = false;
};

template <>
struct is_char<char>{
    constexpr static bool Value = true;
};

template <>
struct is_char<signed char>{
    constexpr static bool Value = true;
};

template <>
struct is_char<unsigned char>{
    constexpr static bool Value = true;
};

template <bool B, typename T = void>
  using enable_if_t = typename std::enable_if<B, T>::type;

template <typename T>
  using remove_reference_t = typename ::std::remove_reference<T>::type;

template <typename T>
  using remove_cv_t = typename ::std::remove_cv<T>::type;

template <typename T>
  using is_integral_t
    = typename TLP_NS::enable_if_t<std::is_integral<T>::value, T>;

template <typename T>
  using not_integral_t
    = typename TLP_NS::enable_if_t<!std::is_integral<T>::value, T>;

template <typename T>
  using is_char_t
    = typename TLP_NS::enable_if_t<TLP_NS::is_char<T>::Value, T>;

template <typename T>
  using not_char_t
    = typename TLP_NS::enable_if_t<!TLP_NS::is_char<T>::Value, T>;

template <typename Base, typename Derived>
  using is_base_of_t
    = typename TLP_NS::enable_if_t<::std::is_base_of<Base, Derived>::value, Base>;
TLP_NS_END

#endif
