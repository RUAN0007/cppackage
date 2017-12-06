#ifndef LOKI_SINGLETON_H
#define LOKI_SINGLETON_H

#include <utility>
#include <stdlib.h>

#include "loki/ThreadModel.h"

LOKI_NS_BEGIN

template <typename T>
struct CreateUsingNew {
  template<typename... Args>
  static T* Create(Args&&... args) {
    return new T(std::forward<Args>(args)...);
  }

  static void Destroy(T* data) {
    delete data;
  }
};

template <typename T>
struct CreateStatic {
  template<typename... Args>
  static T* Create(Args&&... args) {
    static T data{std::forward<Args>(args)...};
    return &data;
  }

  static void Destroy(T* data) {
    // do nothing
  }
};

// For thread safety, pass ClassLevelLockable
// as the template parameter
template <typename T,
          template<typename> class ThreadedPolicy = SingleThreaded,
          template <typename> class CreationPolicy = CreateStatic>
class Singleton : private ThreadedPolicy<T> {
 public:
  // Get instance
  // or init it without constructor parameter
  static T& Instance() {
      return data_ == nullptr ? InstanceWithArg() : *data_;
  }

  // Init instance explicitly with constructor parameters
  template<typename... Args>
  static T& InstanceWithArg(Args&&... args) {
      typename ThreadedPolicy<T>::Lock lock;
      // double check
      if (data_ == nullptr) {
          data_ = CreationPolicy<T>::Create(std::forward<Args>(args)...);
          std::atexit(DestroySingleton);
      }
      return *data_;
  }

 private:
  static void DestroySingleton() {
    CreationPolicy<T>::Destroy(data_);
    data_ = nullptr;
  }
  static T* data_;
};

template <typename T,
      template <typename> class ThreadedPolicy,
      template <typename> class CreationPolicy>
T* Singleton<T, ThreadedPolicy, CreationPolicy>::data_ = nullptr;

LOKI_NS_END

#endif  // LOKI_SINGLETON_H
