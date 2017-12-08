#ifndef LOKI_VISITOR_H
#define LOKI_VISITOR_H

#include "loki/loki.h"

#include "tlp/bool/algo/IfThenElse.h"
#include "tlp/bool/algo/Or.h"
#include "tlp/bool/algo/Not.h"
#include "tlp/bool/BoolType.h"

#include "tlp/list/algo/Length.h"
#include "tlp/list/algo/TypeAt.h"
#include "tlp/list/algo/TypeList.h"
#include "tlp/list/algo/ScatterInherits.h"
#include "tlp/traits/IsConvertible.h"
#include "tlp/int/algo/Sub.h"
#include "tlp/int/IntType.h"

LOKI_NS_BEGIN

template<typename R, typename T>
struct Visitor
{
    virtual R doVisit(T&) = 0;
    virtual ~Visitor() {}
};

template<typename R>
struct VisitorBinder
{
    template<typename T>
    using Result = Visitor<R, T>;
};

template<typename R, typename TL, typename IsLastBase=TLP_NS::BoolType<false>>
struct VisitorGenerator : __scatter_inherits(TL, VisitorBinder<R>::template Result)
{
    using ResultType = R;

    template<typename Visited>
    ResultType visit(Visited& host)
    {
        using ThisPtrType = decltype(this);
        using ParentPtrType = Visitor<R, Visited>*;
        using IsBaseOf = typename TLP_NS::IsConvertible<ThisPtrType, ParentPtrType>::Result;
        using LastIdx = typename TLP_NS::Sub<typename TLP_NS::Length<TL>::Result,
                                              TLP_NS::IntType<1>>::Result;
        using BasePtrType = Visitor<R, typename TLP_NS::TypeAt<TL, LastIdx>::Result>*;
        using Convertible =__or(IsBaseOf, __not(IsLastBase));
        using ConvertedPtrType = typename TLP_NS::IfThenElse<
                                 Convertible, ParentPtrType, BasePtrType>::Result;
        return static_cast<ConvertedPtrType>(this)->doVisit(host);
    }
};

LOKI_NS_END

#define DEFINE_VISITABLE(VISITOR)           \
void accept(VISITOR& visitor) override      \
{                                           \
    visitor.visit(*this);                   \
}

#endif  // LOKI_VISITOR_H
