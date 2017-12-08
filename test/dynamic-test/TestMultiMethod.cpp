
#include "gtest.h"

#include <memory>

#include "loki/MultiMethod.h"

#include "tlp/list/algo/TypeList.h"

struct Digit {
  virtual ~Digit() = default;
};

struct OneDigit : Digit {};
struct TwoDigit : Digit {};
struct ThreeDigit : Digit {};

using DigitList = __type_list(OneDigit, TwoDigit);
using DigitDispatcher1 = LOKI_NS::StaticDispatcher<Digit, DigitList, Digit, DigitList, int>;

struct SumExecutor : DigitDispatcher1::BaseExecutor {
  int Fire(OneDigit&, OneDigit&) override {
    return 2;
  }
  int Fire(TwoDigit&, OneDigit&) override {
    return 3;
  }
  int Fire(OneDigit&, TwoDigit&) override {
    return 3;
  }
  int Fire(TwoDigit&, TwoDigit&) override {
    return 4;
  }

  int OnError(Digit&, Digit&) override {
    return 0;
  }
}; // End SumExecutor


TEST(StaticDispatcher, DynamicExecutor){
  SumExecutor exec;
  Digit* one = new OneDigit;
  Digit* two = new TwoDigit;
  Digit* three = new ThreeDigit;

  ASSERT_EQ(2, DigitDispatcher1::Go(*one, *one, exec));
  ASSERT_EQ(3, DigitDispatcher1::Go(*one, *two, exec));
  ASSERT_EQ(3, DigitDispatcher1::Go(*two, *one, exec));
  ASSERT_EQ(4, DigitDispatcher1::Go(*two, *two, exec));
  ASSERT_EQ(0, DigitDispatcher1::Go(*three, *two, exec));
  ASSERT_EQ(0, DigitDispatcher1::Go(*one, *three, exec));

  delete one;
  delete two;
  delete three;
}


struct StaticSumExecutor {
  static int Fire(OneDigit&, OneDigit&) {
    return 2;
  }
  static int Fire(TwoDigit&, OneDigit&) {
    return 3;
  }
  static int Fire(OneDigit&, TwoDigit&) {
    return 3;
  }
  static int Fire(TwoDigit&, TwoDigit&) {
    return 4;
  }

  static int OnError(Digit&, Digit&) {
    return 0;
  }
};

using DigitDispatcher2 = LOKI_NS::StaticDispatcher<Digit, DigitList, Digit, DigitList, int, StaticSumExecutor>;

TEST(StaticDispatcher, StaticExecutor){
  SumExecutor exec;
  Digit* one = new OneDigit;
  Digit* two = new TwoDigit;
  Digit* three = new ThreeDigit;

  ASSERT_EQ(2, DigitDispatcher2::Go(*one, *one));
  ASSERT_EQ(3, DigitDispatcher2::Go(*one, *two));
  ASSERT_EQ(3, DigitDispatcher2::Go(*two, *one));
  ASSERT_EQ(4, DigitDispatcher2::Go(*two, *two));
  ASSERT_EQ(0, DigitDispatcher2::Go(*one, *three));
  ASSERT_EQ(0, DigitDispatcher2::Go(*three, *two));

  delete one;
  delete two;
  delete three;
}

TEST(FnDispatcher, Basic) {
  Digit* one = new OneDigit;
  Digit* two = new TwoDigit;
  Digit* three = new ThreeDigit;

  LOKI_NS::FnDispatcher<Digit, Digit, int> fn_dispatcher;
  fn_dispatcher.Add<OneDigit, OneDigit, &StaticSumExecutor::Fire>();
  fn_dispatcher.Add<OneDigit, TwoDigit, &StaticSumExecutor::Fire>();
  fn_dispatcher.Add<TwoDigit, OneDigit, &StaticSumExecutor::Fire>();
  fn_dispatcher.Add<TwoDigit, TwoDigit, &StaticSumExecutor::Fire>();
  fn_dispatcher.AddDefault<&StaticSumExecutor::OnError>();

  ASSERT_EQ(2, fn_dispatcher.Go(*one, *one));
  ASSERT_EQ(3, fn_dispatcher.Go(*one, *two));
  ASSERT_EQ(3, fn_dispatcher.Go(*two, *one));
  ASSERT_EQ(4, fn_dispatcher.Go(*two, *two));
  ASSERT_EQ(0, fn_dispatcher.Go(*three, *two));
  ASSERT_EQ(0, fn_dispatcher.Go(*one, *three));

  delete one;
  delete two;
  delete three;
}

