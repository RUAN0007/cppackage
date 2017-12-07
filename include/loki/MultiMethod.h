#ifndef LOKI_MULTIMETHOD_H
#define LOKI_MULTIMETHOD_H

#include <typeinfo>

#include "loki/loki.h"
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


LOKI_NS_END

#endif
