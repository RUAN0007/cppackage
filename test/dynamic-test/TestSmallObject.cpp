
#include "gtest.h"

#include <iostream>
#include <memory>
#include <vector>

#include "loki/SmallObject.h"
#include "utils/Timer.h"

struct A_t {
  char a_[4];
};

struct A_st : LOKI_NS::SmallObject<> {
  char a_[4];
};

struct B_t {
  char b_[8];
};

struct B_st : LOKI_NS::SmallObject<> {
  char b_[4];
};

struct C_t {
  char c_[16];
};

struct C_st : LOKI_NS::SmallObject<> {
  char c_[16];
};

template<class A, class B, class C>
void CreateDestroy(A a, B b, C c, size_t counter) {

  std::vector<A*> Av;
  std::vector<B*> Bv;
  std::vector<C*> Cv;

  for (size_t i = 0; i < counter; ++i) {
    Av.push_back(new A);
  }

  for (size_t i = 0; i < counter; ++i) {
    delete Av[i];
  }

  for (size_t i = 0; i < counter; ++i) {
    Bv.push_back(new B);
  }

  for (size_t i = 0; i < counter; ++i) {
    Cv.push_back(new C);
  }

  for (size_t i = 0; i < counter; ++i) {
    delete Cv[i];
  }

  for (size_t i = 0; i < counter; ++i) {
    delete Bv[i];
  }
};

TEST(SmallObject, Basic){
  A_t at;
  B_t bt;
  C_t ct;

  A_st ast;
  B_st bst;
  C_st cst;


  UTILS_NS::TimerPool::Instance()->GetTimer("Non-Small-Object").Start();
  CreateDestroy(at, bt, ct, 1000000);
  UTILS_NS::TimerPool::Instance()->GetTimer("Non-Small-Object").Stop();

  UTILS_NS::TimerPool::Instance()->GetTimer("Small-Object").Start();
  CreateDestroy(ast, bst, cst, 1000000);
  UTILS_NS::TimerPool::Instance()->GetTimer("Small-Object").Stop();

}
