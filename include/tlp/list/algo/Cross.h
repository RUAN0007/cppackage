#ifndef TLP_LIST_ALGO_CROSS
#define TLP_LIST_ALGO_CROSS

#include "tlp/base/NullType.h"

#include "tlp/list/algo/Append.h"
#include "tlp/list/algo/Map.h"
#include "tlp/list/TypeElem.h"
#include "tlp/bool/algo/IfThenElse.h"
#include "tlp/bool/algo/IsEqual.h"


TLP_NS_BEGIN

template<typename T1, typename T2>
struct Pair {
  using First = T1;
  using Second = T2;
};

template<typename TL1, typename TL2> struct Cross;

template<typename R>
struct PairBinder
{
    template<typename T>
    struct Bind {
      using Result = Pair<R, T>;
    };
};

template<>
struct Cross<NullType, NullType>
{
    using Result = NullType;
};


template<typename TL2>
struct Cross<NullType, TL2>
{
    using Result = NullType;
};

template<typename Head1, typename TL2>
struct Cross<TypeElem<Head1, NullType>, TL2>
{
    using Result = typename Map<TL2,
                                PairBinder<Head1>::template Bind>::Result;
};

template<typename Head1, typename Tail1, typename TL2>
struct Cross<TypeElem<Head1, Tail1>, TL2>
{
  private:
    using HeadCross = typename Cross<TypeElem<Head1, NullType>, TL2>::Result;
    using TailCross = typename Cross<Tail1, TL2>::Result;
    using NotNullTL2Result = typename Append<HeadCross, TailCross>::Result;
  public:
  using Result = typename IfThenElse<typename IsEqual<TL2, NullType>::Result,
                                     NullType, NotNullTL2Result>::Result;
};

TLP_NS_END

#define __cross(...)   typename TLP_NS::Cross<__VA_ARGS__>::Result
#define __pair(...)    typename TLP_NS::Pair<__VA_ARGS__>
#endif
