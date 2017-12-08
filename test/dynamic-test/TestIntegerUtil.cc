#include "gtest.h"

#include "utils/Integer.h"

TEST(AppendRead, VariousType) {
  char* nums = new char[30];
  int a = 1;
  long b = 2;
  short c = 3;
  size_t expected_length = sizeof(a) + sizeof(b) + sizeof(c);
  ASSERT_EQ(expected_length,
            UTILS_NS::AppendInteger(nums, a, b, c));

  int ea;
  long eb;
  short ec;

  ASSERT_EQ(expected_length,
            UTILS_NS::ReadInteger(nums, ea, eb, ec));
  ASSERT_EQ(a, ea);
  ASSERT_EQ(b, eb);
  ASSERT_EQ(c, ec);
  delete[] nums;
}

TEST(AppendRead, ArrayRead) {
  int i0 = 1;
  int i1 = 2;
  int i2 = 3;
  int i[3];

  char* nums = new char[30];
  size_t expected_length = 3 * sizeof(i0);
  ASSERT_EQ(expected_length,
            UTILS_NS::AppendInteger(nums, i0, i1, i2));

  ASSERT_EQ(expected_length,
            UTILS_NS::ReadInteger(nums, i));
  ASSERT_EQ(i0, i[0]);
  ASSERT_EQ(i1, i[1]);
  ASSERT_EQ(i2, i[2]);

  delete[] nums;
}

TEST(AppendRead, PointerAppend) {
  int i0;
  int i1;
  int i2;
  int* i = new int[3]{1, 2, 3};
  char* nums = new char[30];
  size_t expected_length = 3 * sizeof(i0);
  size_t actual_legnth = UTILS_NS::AppendInteger<int, 3>(nums, i);
  ASSERT_EQ(expected_length, actual_legnth);

  ASSERT_EQ(expected_length,
            UTILS_NS::ReadInteger(nums, i0, i1, i2));
  ASSERT_EQ(i[0], i0);
  ASSERT_EQ(i[1], i1);
  ASSERT_EQ(i[2], i2);

  delete[] i;
  delete[] nums;
}

TEST(AppendRead, ArrayAppendPointerRead) {
  int append_nums[3] = {1, 2, 3};
  int* read_nums = new int[3];
  char* nums = new char[30];
  size_t expected_length = 3 * sizeof(int);
  size_t actual_length = UTILS_NS::AppendInteger(nums, append_nums);
  ASSERT_EQ(expected_length, actual_length);

  actual_length = UTILS_NS::ReadInteger<int, 3>(nums, read_nums);
  ASSERT_EQ(expected_length, actual_length);
  ASSERT_EQ(read_nums[0], append_nums[0]);
  ASSERT_EQ(read_nums[1], append_nums[1]);
  ASSERT_EQ(read_nums[2], append_nums[2]);

  delete[] read_nums;
  delete[] nums;
}
