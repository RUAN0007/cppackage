#ifndef LOKI_MULTIMETHOD_H
#define LOKI_MULTIMETHOD_H

#include <map>

#include "loki/loki.h"
#include "loki/TypeInfo.h"
#include "tlp/base/NullType.h"
#include "tlp/list/algo/ScatterInherits.h"
#include "tlp/list/algo/Cross.h"
#include "tlp/int/IntType.h"

LOKI_NS_BEGIN

template <class ReturnType>
struct ExecutorBinder {
  template <class ArgPair>
  struct Executor {
    virtual ReturnType Fire(typename ArgPair::First&,
                            typename ArgPair::Second&) = 0;
    virtual ~Executor() = default;
  };
};

template <
   class BaseLhs,
   class TypesLhs,
   class BaseRhs = BaseLhs,
   class TypesRhs = TypesLhs,
   typename ResultType = void>
struct AbstractExecutor : TLP_NS::ScatterInherits<
    typename TLP_NS::Cross<TypesLhs, TypesRhs>::Result,
    ExecutorBinder<ResultType>::template Executor> {
  virtual ResultType OnError(BaseLhs&, BaseRhs&) = 0;
  virtual ~AbstractExecutor() = default;
};

template <
   class BaseLhs,
   class TypesLhs,
   class BaseRhs = BaseLhs,
   class TypesRhs = TypesLhs,
   typename ResultType = void,
   class Executor = AbstractExecutor<BaseLhs, TypesLhs, BaseRhs, TypesRhs, ResultType>>
class StaticDispatcher {
  using Head = typename TypesLhs::Head;
  using Tail = typename TypesLhs::Tail;
 public:
  using BaseExecutor = Executor;
  static ResultType Go(BaseLhs& lhs, BaseRhs& rhs,
                       Executor& exec) {
    if (Head* p1 = dynamic_cast<Head*>(&lhs)) {
      return StaticDispatcher<BaseLhs, TLP_NS::NullType,
                              BaseRhs, TypesRhs,
                              ResultType, Executor>
                              ::DispatchRhs(*p1, rhs, exec);
    } else {
      return StaticDispatcher<BaseLhs, Tail,
                              BaseRhs, TypesRhs,
                              ResultType, Executor>::Go(
                              lhs, rhs, exec);
    }  // end if
  }  // end go()

  static ResultType Go(BaseLhs& lhs, BaseRhs& rhs) {
    if (Head* p1 = dynamic_cast<Head*>(&lhs)) {
      return StaticDispatcher<BaseLhs, TLP_NS::NullType,
                              BaseRhs, TypesRhs,
                              ResultType, Executor>
                              ::DispatchRhs(*p1, rhs);
    } else {
      return StaticDispatcher<BaseLhs, Tail,
                              BaseRhs, TypesRhs,
                              ResultType, Executor>::Go(
                              lhs, rhs);
    }  // end if
  }  // end go()
};

template <
   class BaseLhs,
   class BaseRhs,
   class TypesRhs,
   typename ResultType,
   class Executor>
class StaticDispatcher<BaseLhs, TLP_NS::NullType,
   BaseRhs, TypesRhs, ResultType, Executor> {
 public:
  static ResultType Go(BaseLhs& lhs, BaseRhs& rhs, Executor& exec) {
      return exec.OnError(lhs, rhs);
   }  // end Go

  static ResultType Go(BaseLhs& lhs, BaseRhs& rhs) {
      return Executor::OnError(lhs, rhs);
   }  // end Go

  template <class SomeLhs>
  static ResultType DispatchRhs(SomeLhs& lhs, BaseRhs& rhs,
      Executor& exec) {
    using Head = typename TypesRhs::Head;
    using Tail = typename TypesRhs::Tail;
    if (Head* p2 = dynamic_cast<Head*>(&rhs)) {

       using BaseExecutor = typename ExecutorBinder<ResultType>::template
                              Executor<TLP_NS::Pair<SomeLhs, Head>>;
       return static_cast<BaseExecutor&>(exec).Fire(lhs, *p2);
       // return exec.Fire(lhs, *p2);
    } else {
      return StaticDispatcher<BaseLhs, TLP_NS::NullType,
                              BaseRhs, Tail,
                              ResultType, Executor>::DispatchRhs(
      lhs, rhs, exec);
    }  // end if
  }  // end DispatchRhs

  template <class SomeLhs>
  static ResultType DispatchRhs(SomeLhs& lhs, BaseRhs& rhs) {
    using Head = typename TypesRhs::Head;
    using Tail = typename TypesRhs::Tail;
    if (Head* p2 = dynamic_cast<Head*>(&rhs)) {
      return Executor::Fire(lhs, *p2);
    } else {
      return StaticDispatcher<BaseLhs, TLP_NS::NullType,
                              BaseRhs, Tail,
                              ResultType, Executor>::DispatchRhs(
        lhs, rhs);
    }  // end if
  }  // end DispatchRhs
};

template <
   class BaseLhs,
   class BaseRhs,
   typename ResultType,
   class Executor>
class StaticDispatcher<BaseLhs, TLP_NS::NullType,
   BaseRhs, TLP_NS::NullType, ResultType, Executor> {
 public:
  static ResultType DispatchRhs(BaseLhs& lhs, BaseRhs& rhs,
      Executor& exec) {
      return exec.OnError(lhs, rhs);
   }  // end void

   static ResultType DispatchRhs(BaseLhs& lhs, BaseRhs& rhs) {
      return Executor::OnError(lhs, rhs);
   }  // end void
};


template <class BaseLhs, class BaseRhs = BaseLhs, typename ResultType = void>
class BasicDispatcher {
 private:
  using KeyType = std::pair<TypeInfo, TypeInfo>;
  using CallbackType = ResultType (*)(BaseLhs&, BaseRhs&);
  using MappedType = CallbackType;
  using MapType = std::map<KeyType, MappedType> ;
  MapType callbackMap_;
  CallbackType default_func_;

 public:
  void AddDefault(CallbackType fun) {
    default_func_ = fun;
  }

  template <class SomeLhs, class SomeRhs>
  void Add(CallbackType fun) {
    const KeyType key(typeid(SomeLhs), typeid(SomeRhs));
    callbackMap_[key] = fun;
   }  // End Add

  ResultType Go(BaseLhs& lhs, BaseRhs& rhs) {
    typename MapType::iterator i = callbackMap_.find(
                            KeyType(typeid(lhs), typeid(rhs)));
      if (i == callbackMap_.end()) {
        return default_func_(lhs, rhs);
      } else {
        return (i->second)(lhs, rhs);
      }  // End if
   }  // End Go
};

template <class BaseLhs, class BaseRhs = BaseLhs,
          class ResultType = void>
class FnDispatcher {
 private:
  BasicDispatcher<BaseLhs, BaseRhs, ResultType> backEnd_;
  using DefaultCallbackType = ResultType (*)(BaseLhs&, BaseRhs&);

 public:
  template <class ConcreteLhs, class ConcreteRhs,
            ResultType (*callback)(ConcreteLhs&, ConcreteRhs&)>
  void Add() {
    struct Local {
      static ResultType Trampoline(BaseLhs& lhs, BaseRhs& rhs) {
            return callback(
               static_cast<ConcreteLhs&>(lhs),
               static_cast<ConcreteRhs&>(rhs));
      };
    };
    return backEnd_.template Add<ConcreteLhs, ConcreteRhs>(&Local::Trampoline);
  }

  template <ResultType (*default_call)(BaseLhs&, BaseRhs&)>
  void AddDefault() {
    return backEnd_.AddDefault(default_call);
  }

  ResultType Go(BaseLhs& lhs, BaseRhs& rhs) {
    return backEnd_.Go(lhs, rhs);
  }
};

LOKI_NS_END

#endif
