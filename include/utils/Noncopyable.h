#ifndef UTILS_NONCOPYABLE_H
#define UTILS_NONCOPYABLE_H

#include <iterator>

#include "utils/utils.h"

UTILS_NS_BEGIN

struct Noncopyable {
  Noncopyable() = default;
  ~Noncopyable() = default;
  Noncopyable(const Noncopyable&) = delete;
  Noncopyable& operator=(const Noncopyable&) = delete;
};

struct Moveable : private Noncopyable {
  Moveable() = default;
  ~Moveable() = default;
  Moveable(Moveable&&) {}
  Moveable& operator=(Moveable&&) { return *this; }
};

UTILS_NS_END
#endif  // UTILS_NONCOPYABLE_H
