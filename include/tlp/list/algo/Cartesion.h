#ifndef TLP_LIST_ALGO_CARTESION
#define TLP_LIST_ALGO_CARTESION

#include "tlp/base/NullType.h"

#include "tlp/list/algo/Cross.h"
#include "tlp/bool/algo/IfThenElse.h"

TLP_NS_BEGIN

template<typename HList, typename ...TLists> struct Cartesion;

template<typename HList1, typename HList2, typename ...TLists>
struct Cartesion<HList1, HList2, TLists...> {
  private:
    using TailResult = typename Cartesion<HList2, TLists...>::Result;
    using NonNullHList1Result =
        typename Cross<HList1, TailResult>::Result;
  public:
    using Result = typename IfThenElse<typename IsEqual<HList1, NullType>::Result,                                     TailResult, NonNullHList1Result>::Result;
};

template<typename HList1, typename HList2>
struct Cartesion<HList1, HList2> {
  using Result = typename Cross<HList1, HList2>::Result;
};

template<typename HList>
struct Cartesion<HList> {
  using Result = typename Cross<HList, NullType>::Result;
};

TLP_NS_END

#define __cartesion(...)   typename TLP_NS::Cartesion<__VA_ARGS__>::Result

#endif
