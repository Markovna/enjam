#include <enjector/enjector.h>
#include <enjector/factories.h>
#include <iostream>


int main() {
  using namespace enjector;

  class Foo {
   public:
    Foo() {
      std::cout << "Foo constructor\n";
    }

    Foo(const Foo&) = delete;

    virtual ~Foo() {
      std::cout << "Foo destructor\n";
    }

  };

  class BarDep {
   public:
    BarDep() {
      std::cout << "BarDep constructor\n";
    }

    ~BarDep() {
      std::cout << "BarDep destructor\n";
    }
  };

  class Bar : public Foo {
   public:
    Bar() = delete;
    Bar(BarDep&) {
      std::cout << "Bar di constructor\n";
    }

    ~Bar() {
      std::cout << "Bar destructor\n";
    }
  };

  class NoDefaultConstructor {
   public:
    NoDefaultConstructor() = delete;
    NoDefaultConstructor(Foo&) {}
  };

  {
    std::cout << "<============== Test calling Bar di constructor \n";
    Dependencies<Foo, BarDep> a{};
    a.bind<Foo>().to<Bar(BarDep&)>();
    auto foo = a.resolve<Foo*>();
    assert(foo);
  }

  {
    std::cout << "<============== Test binding to reference\n";
    Foo foo {};
    Dependencies<Foo, BarDep> a{};
    a.bind<Foo>().to(foo);
    auto& fooResolved = a.resolve<Foo&>();

  }

  {
    std::cout << "<============== Test Valid after move \n";
    Dependencies<Foo, BarDep> a{};
    a.bind<Foo>().to<Bar(BarDep&)>();

    auto newDeps = std::move(a);
    auto foo = newDeps.resolve<Foo*>();
    assert(foo);
  }

  {
    std::cout << "<============== Test no default constructor compiles \n";
    Dependencies<NoDefaultConstructor, Foo> a{};
    a.bind<NoDefaultConstructor>().to<NoDefaultConstructor(Foo&)>();
    a.bind<Foo>().to<Foo>();
  }

  {
    std::cout << "<============== Test clear \n";
    Dependencies<Foo, BarDep> a{};
    a.bind<Foo>().to<Bar(BarDep&)>();
    a.clear();
  }

  {
    std::cout << "<============== Test factories \n";
    Dependencies<IFactory<Foo>, BarDep> a {};
    a.bind<IFactory<Foo>>().to<Factory<Bar(), BarDep&>>();

    auto& factory = a.resolve<IFactory<Foo>&>();
    auto foo = factory();
    assert(foo);
  }
}