#ifndef HDD5F16F3_609B_4ADD_8AFD_D8841305227D
#define HDD5F16F3_609B_4ADD_8AFD_D8841305227D

#include "tlp/tlp.h"

TLP_NS_BEGIN

template<typename V>
struct TypeType
{
    using OriginalType = V;
    using Result = TypeType<V>;
};

TLP_NS_END

#define __type(value)    typename TLP_NS::TypeType<value>::Result

#endif
