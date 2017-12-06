#ifndef LOKI_FACTORY_H
#define LOKI_FACTORY_H

#include "loki/loki.h"
#include "loki/TypeInfo.h"

#include "tlp/list/algo/LinearInherits.h"
#include "tlp/list/algo/Reverse.h"
#include "tlp/list/algo/ScatterInherits.h"
#include "tlp/base/TypeType.h"

#include <map>
#include <memory>
#include <utility>

LOKI_NS_BEGIN

template<
class AbstractProduct,
class IdentifierType,
class ProductCreator = AbstractProduct* (*)(IdentifierType)>
    class Factory {
 public:
  Factory() = default;

  bool Register(const IdentifierType& id, ProductCreator creator) {
    return associations_.insert(
              std::make_pair(id, creator)).second;
  }

  bool Unregister(const IdentifierType& id) {
    return associations_.erase(id) == 1;
  }

  std::unique_ptr<AbstractProduct> CreateObject(
      const IdentifierType& id) {
    std::unique_ptr<AbstractProduct> result;
    typename AssocMap::const_iterator i =
       associations_.find(id);
    if (i != associations_.end()){
      result.reset((i->second)(id));
    }  // end if
    return result;
  }

 private:
  using AssocMap = std::map<IdentifierType, ProductCreator>;
  AssocMap associations_;
};

template <class T>
class AbstractFactoryUnit {
 public:
  virtual T* DoCreate(TLP_NS::TypeType<T>) = 0;
  virtual ~AbstractFactoryUnit() = default;
};


template
<class TList,
template <class> class Unit = AbstractFactoryUnit>
class AbstractFactory : public TLP_NS::ScatterInherits<TList, Unit> {
 public:
  using ProductList = TList;

  template <typename T>
  std::unique_ptr<T> Create() {
    Unit <T>& unit = *this;
    return std::unique_ptr<T>(
        unit.DoCreate(TLP_NS::TypeType<T>()));
  }

  ~AbstractFactory() = default;
};

template <class ConcreteProduct, class Base>
class OpNewFactoryUnit : public Base {
  using BaseProductList = typename Base::ProductList;
 protected:
  using ProductList = typename BaseProductList::Tail;
 public:
  using AbstractProduct = typename BaseProductList::Head;

  ConcreteProduct* DoCreate(TLP_NS::TypeType<AbstractProduct>)
    override {
      return new ConcreteProduct();
  }
  virtual ~OpNewFactoryUnit() = default;
};

template <
class AbstractFact,
class TList = typename AbstractFact::ProductList,
template <class, class> class Creator = OpNewFactoryUnit>
class ConcreteFactory : public TLP_NS::LinearInherits<
   typename TLP_NS::Reverse<TList>::Result, Creator, AbstractFact> {
 public:
  using ProductList = typename AbstractFact::ProductList;
  using ConcreteProductList = TList;
};

LOKI_NS_END

#endif
