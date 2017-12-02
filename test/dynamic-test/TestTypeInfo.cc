#include "gtest.h"

#include "loki/TypeInfo.h"

USING_LOKI_NS

TEST(TypeInfo, Normal) {
  struct A{} a;
  struct B{} b;
  struct C : A{} c;

  using AA = A;
  AA aa;

  TypeInfo ta(typeid(a));
  TypeInfo taa(typeid(aa));
  TypeInfo tb(typeid(b));
  TypeInfo tc(typeid(c));

  ASSERT_EQ(ta, ta);
  ASSERT_EQ(ta, taa);
  ASSERT_NE(ta, tb);
  ASSERT_LT(ta, tb);
  ASSERT_LE(ta, taa);
  ASSERT_GT(tc, ta);
}
