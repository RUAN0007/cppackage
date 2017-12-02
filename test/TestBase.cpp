#include "tlp/test/Test.hpp"
#include "tlp/base/EmptyType.h"
#include "tlp/base/NullType.h"
#include "tlp/base/TypeType.h"
#include "tlp/base/algo/Valid.h"

FIXTURE(TestBase)
{
    TEST("null type should not be valid")
    {
        ASSERT_FALSE(__valid(__null()));
    };

    TEST("empty type should be valid")
    {
        ASSERT_TRUE(__valid(__empty()));
    };

    TEST("empty should not be equal to null")
    {
        ASSERT_NE(__empty(), __null());
    };

    TEST("type to the same type shoud be equal") {
        struct A;
        using AA = A;
        struct B;
        ASSERT_EQ(__type(A), __type(AA));
        ASSERT_NE(__type(A), __type(B));
    };
};
