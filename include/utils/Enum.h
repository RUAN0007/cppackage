#ifndef UTILS_ENUM_H
#define UTILS_ENUM_H

#include <iterator>

#include "utils/utils.h"
#include "tlp/traits/TypeTraits.h"

UTILS_NS_BEGIN

template <typename T, typename = TLP_NS::enable_if_t<std::is_enum<T>::value>>
struct Enum {
  class Iterator : public std::iterator<std::input_iterator_tag, T> {
   public:
    explicit Iterator(int pos) : pos_(pos) {}

    Iterator& operator++() {
      ++pos_;
      return *this;
    }

    Iterator operator++(int) {
      Iterator it = *this;
      ++(*this);
      return it;
    }

    T operator*() const {
      return (T)pos_;
    }

    Iterator(const Iterator&) noexcept = default;
    Iterator& operator=(const Iterator&) noexcept = default;
    bool operator==(const Iterator& other) const { return pos_ == other.pos_; }
    bool operator!=(const Iterator& other) const { return !(*this == other); }

   private:
     int pos_;
  };
};

template <typename T, typename = TLP_NS::enable_if_t<std::is_enum<T>::value>>
typename Enum<T>::Iterator begin(Enum<T>) {
  return typename Enum<T>::Iterator(static_cast<int>(T::First));
}

template <typename T, typename = TLP_NS::enable_if_t<std::is_enum<T>::value>>
typename Enum<T>::Iterator end(Enum<T>) {
  return typename Enum<T>::Iterator(static_cast<int>(T::Last) + 1);
}

UTILS_NS_END
#endif  // UTILS_ENUM_H_
