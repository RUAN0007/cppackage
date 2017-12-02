#ifndef H1FA6AF36_DFE1_4640_823A_39D9A921D312
#define H1FA6AF36_DFE1_4640_823A_39D9A921D312

#include "Points.h"

#include "tlp/func/Forward.h"
#include "tlp/list/TypeElem.h"
#include "tlp/list/algo/Belong.h"
#include "tlp/bool/algo/Not.h"
#include "tlp/bool/algo/And.h"

namespace triangle
{
    template<typename Triple, typename Figure> struct IsTriangle;

    template<typename P1, typename P2, typename P3, typename Figure>
    struct IsTriangle<__type_elem(P1, __type_elem(P2, __type_elem(P3, __null()))), Figure>
    {
    private:
        __func_forward_2(Connected, __belong(__points(_1, _2), Figure));
        __func_forward_3(InSameLine, __belong(__points(_1, _2, _3), Figure));

    public:
        using Result = __and( Connected<P1, P2>
                            , __and(Connected<P2, P3>
                                   , __and(Connected<P3, P1>, __not(InSameLine<P1, P2, P3>))));
    };
}

#define __is_triangle(...)  typename triangle::IsTriangle<__VA_ARGS__>::Result


#endif
