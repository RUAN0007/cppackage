#ifndef LOKI_SINGLETON_H
#define LOKI_SINGLETON_H

#include <utility>

#include "loki/ThreadModel.h"

LOKI_NS_BEGIN
// For thread safety, pass ClassLevelLockable
// as the template parameter
template <typename T,
      template<typename> class ThreadedPolicy = SingleThreaded>
class Singleton : private ThreadedPolicy<T> {
 public:
  // Get instance
  // or init it without constructor parameter
  static T* Instance() {
      return data_ == nullptr ? MakeSingleton() : data_;
  }

  // Init instance explicitly with constructor parameters
  template<typename... Args>
  static T* MakeSingleton(Args&&... args) {
      typename ThreadedPolicy<T>::Lock lock;
      // double check
      if (data_ == nullptr) {
          static T data{std::forward<Args>(args)...};
          data_ = &data;
      }
      return data_;
  }

 private:
  static T* data_;
};

template <typename T,
      template<typename> class ThreadedPolicy>
T* Singleton<T, ThreadedPolicy>::data_ = nullptr;

LOKI_NS_END

#endif  // LOKI_SINGLETON_H
