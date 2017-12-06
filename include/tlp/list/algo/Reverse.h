#ifndef TLP_LIST_ALGO_REVERSE_H
#define TLP_LIST_ALGO_REVERSE_H

#include "tlp/tlp.h"
#include "tlp/base/NullType.h"
#include "tlp/list/TypeElem.h"
#include "tlp/list/algo/Append.h"

TLP_NS_BEGIN

template<typename TL> struct Reverse;

template<>
struct Reverse<NullType>
{
    using Result = NullType;
};

template<typename Head>
struct Reverse<TypeElem<Head, NullType>>
{
    using Result = TypeElem<Head, NullType>;
};

template<typename Head, typename Tail>
struct Reverse<TypeElem<Head, Tail>>
{
  public:
    using Result = typename Append<typename Reverse<Tail>::Result, Head>::Result;
};

TLP_NS_END

#define __reverse(...) typename TLP_NS::Reverse<__VA_ARGS__>::Result

#endif
