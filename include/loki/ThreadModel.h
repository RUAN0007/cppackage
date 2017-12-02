#ifndef LOKI_THREAD_MODEL_H
#define LOKI_THREAD_MODEL_H

#include <mutex>

#include "loki/loki.h"

LOKI_NS_BEGIN

template <typename T>
struct SingleThreaded {
 protected:
  SingleThreaded() = default;

  struct Lock : private UTILS_NS::Noncopyable {
    Lock() = default;
    explicit Lock(const SingleThreaded&) : Lock() {}
    explicit Lock(const SingleThreaded*) : Lock() {}
    ~Lock() = default;
  };
};

template <typename T>
struct ObjectLevelLockable {
 protected:
  ObjectLevelLockable() = default;

  struct Lock {
    // should be noncopyable; otherwise deadlock occurs in copy ctor
    // and assignment operator
    Lock() = delete;
    explicit Lock(const ObjectLevelLockable& host) : host_(host) {
      host_.mtx_.lock();
    }
    explicit Lock(const ObjectLevelLockable* host) : Lock(*host) {}
    ~Lock() { host_.mtx_.unlock(); }

   private:
    const ObjectLevelLockable& host_;
  };

 private:
  mutable std::mutex mtx_;
  friend class Lock;
};

template <typename T>
struct ClassLevelLockable {
 protected:
  ClassLevelLockable() = default;

  struct Lock {
    // should be noncopyable; otherwise deadlock occurs in copy ctor
    // and assignment operator
    Lock() { ClassLevelLockable::mtx_.lock(); }
    explicit Lock(const ClassLevelLockable& host) : Lock() {}
    explicit Lock(const ClassLevelLockable* host) : Lock(*host) {}
    ~Lock() { ClassLevelLockable::mtx_.unlock(); }
  };

 private:
  static std::mutex mtx_;
  friend class Lock;
};

template <typename T>
std::mutex ClassLevelLockable<T>::mtx_;

LOKI_NS_END

#endif  // LOKI_THREAD_MODEL_H
