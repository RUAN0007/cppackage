
#include "gtest.h"

#include <iostream>
#include <memory>

#include "loki/Visitor.h"

struct Animal;
struct Chick;
struct Rabbit;
struct Snake;
struct Dog;

using AnimalCounterVisitor = LOKI_NS::VisitorGenerator<
  /*Put the base visitable at the end of list for visitor
    when encountering the unknown child visitable*/
  void, __type_list(Chick, Rabbit, Snake, Animal), TLP_NS::BoolType<true>>;

struct Animal {
  virtual void accept(AnimalCounterVisitor&) = 0;
  virtual ~Animal() {}
};

struct Chick : Animal {
 private:
   DEFINE_VISITABLE(AnimalCounterVisitor);
};

struct Rabbit : Animal {
 private:
   DEFINE_VISITABLE(AnimalCounterVisitor);
};

struct Snake : Animal {
 private:
  DEFINE_VISITABLE(AnimalCounterVisitor);
};

struct Dog : Animal {
 private:
  DEFINE_VISITABLE(AnimalCounterVisitor);
};

struct HeadCounterVisitor : AnimalCounterVisitor {
 public:
   int count_ = 0;
 private:
  void doVisit(Chick&) override {
    count_++;
  }

  void doVisit(Rabbit&) override {
    count_++;
  }

  void doVisit(Snake&) override {
    count_++;
  }

  void doVisit(Animal&) override {
    count_++;
  }
};

struct LegCounterVisitor : AnimalCounterVisitor {
 public:
   int count_ = 0;
 private:
  void doVisit(Chick&) override {
    std::cout << "Chich Leg" << std::endl;
    count_ += 2;
  }

  void doVisit(Rabbit&) override {
    std::cout << "Rabbit Leg" << std::endl;
    count_ += 4;
  }

  void doVisit(Snake&) override {
    // do nothing
  }

  void doVisit(Animal&) override {
    std::cout << "Animal Leg" << std::endl;

    // do nothing
  }
};


TEST(Visitor, Basic){
  HeadCounterVisitor head_counter;
  LegCounterVisitor leg_counter;

  Animal* animals[] = {new Chick, new Rabbit, new Snake, new Dog};

  for(auto animal : animals) {
    animal->accept(head_counter);
    animal->accept(leg_counter);
  }

  ASSERT_EQ(4, head_counter.count_);
  ASSERT_EQ(6, leg_counter.count_);

  for(auto animal : animals) delete animal;
}
