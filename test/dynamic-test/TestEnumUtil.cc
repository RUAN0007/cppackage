#include "gtest.h"

#include "utils/Enum.h"

enum Color {
  kRED, kBLUE, kGREEN,
  First=kRED, Last=kGREEN,
};

TEST(EnumIt, Begin) {
  auto it = UTILS_NS::begin(UTILS_NS::Enum<Color>());
  ASSERT_EQ(Color::kRED, *it);
  ++it;
  ASSERT_EQ(Color::kBLUE, *it);
  auto old_it = it++;
  ASSERT_EQ(Color::kBLUE, *old_it);
  ASSERT_EQ(Color::kGREEN, *it);
  it++;
  auto endit = UTILS_NS::end(UTILS_NS::Enum<Color>());
  ASSERT_EQ(it, endit);
}
