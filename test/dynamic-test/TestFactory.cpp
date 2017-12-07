
#include "gtest.h"

#include <memory>

#include "loki/Factory.h"

#include "tlp/list/algo/TypeList.h"

struct Base {
  Base() {}
  virtual int value() {
    return 0;
  }
};

struct Derived1: Base {
  Derived1() {}
  int value() override {
    return 1;
  }
};

struct Derived2: Base {
  Derived2() {}
  int value() override {
    return 2;
  }
};

Base* Create1(int) {
  return new Derived1();
}

Base* Create2(int) {
  return new Derived2();
}

TEST(Factory, Basic){
  using BasicFactory = LOKI_NS::Factory<Base, int>;
  BasicFactory factory;

  ASSERT_TRUE(factory.Register(1, &Create1));
  ASSERT_TRUE(factory.Register(2, &Create2));

  auto base1 = factory.CreateObject(1);
  auto base2 = factory.CreateObject(2);

  ASSERT_EQ(1, base1->value());
  ASSERT_EQ(2, base2->value());
}

struct BaseA {
  BaseA() {}
  virtual char value() {
    return 'a';
  };
};

struct DerivedA: BaseA {
  DerivedA() {}
  virtual char value() override {
    return 'A';
  }
};

struct BaseB {
  BaseB() {}
  virtual char value() {
    return 'b';
  }
};

struct DerivedB: BaseB {
  DerivedB() {}
  virtual char value() override {
    return 'B';
  }
};

TEST(AbstractFactotry, Basic) {
  using BasicAbstractFactory = LOKI_NS::AbstractFactory<__type_list(BaseA, BaseB)>;

  BasicAbstractFactory* concreteFactory1 =
      new LOKI_NS::ConcreteFactory<BasicAbstractFactory>;
  ASSERT_EQ('a', concreteFactory1->Create<BaseA>()->value());
  ASSERT_EQ('b', concreteFactory1->Create<BaseB>()->value());

  BasicAbstractFactory* concreteFactory2 =
      new LOKI_NS::ConcreteFactory<BasicAbstractFactory, __type_list(DerivedA, DerivedB)>;
  ASSERT_EQ('A', concreteFactory2->Create<BaseA>()->value());
  ASSERT_EQ('B', concreteFactory2->Create<BaseB>()->value());

  delete concreteFactory1;
  delete concreteFactory2;
}
