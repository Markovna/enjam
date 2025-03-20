#include "njctr/njctr.h"
#include "test.h"

using namespace njctr;

struct interface1 {
  virtual void dummy() = 0;
  virtual ~interface1() = default;
};

struct interface2 {
  virtual void dummy() = 0;
  virtual ~interface2() = default;};

struct interface3 {
  virtual void dummy() = 0;
  virtual ~interface3() = default;
};

struct impl1_without_deps : interface1 {
  impl1_without_deps() = default;
  explicit impl1_without_deps(int value) : value(value) { }

  void dummy() override { }
  int value = 0;
};

struct impl1_with_deps : interface1 {
  explicit impl1_with_deps(interface2&) { }
  explicit impl1_with_deps(interface3&) { }
  impl1_with_deps(interface2&, int value) : value(value) { }

  void dummy() override { }
  int value = 0;
};

Test test_shared_resolved_types = [] {
  auto injector = Injector<SharedResolver<interface1>, SharedResolver<interface2>>();
  injector.bind<interface1>().to<impl1_without_deps>();
  assert(dynamic_cast<impl1_without_deps*>(&injector.resolve<interface1&>()));
  assert(dynamic_cast<impl1_without_deps*>(injector.resolve<std::shared_ptr<interface1>>().get()));

  injector.bind<interface1>().to<impl1_with_deps(interface2&)>();
  assert(dynamic_cast<impl1_with_deps*>(&injector.resolve<interface1&>()));
  assert(dynamic_cast<impl1_with_deps*>(injector.resolve<std::shared_ptr<interface1>>().get()));

  injector.bind<interface1>().to(std::make_shared<impl1_without_deps>());
  assert(dynamic_cast<impl1_without_deps*>(&injector.resolve<interface1&>()));
  assert(dynamic_cast<impl1_without_deps*>(injector.resolve<std::shared_ptr<interface1>>().get()));

//  injector.bind<interface1>().to<impl1_with_deps(interface3&)>(); // compile error, interface3 can't be resolved
};

Test test_factory_without_args_resolved_types = [] {
  auto injector = Injector<IFactory<interface1()>, SharedResolver<interface2>>();
  injector.bind<IFactory<interface1()>>().to<Factory<impl1_without_deps()>>();
  {
    auto factory = injector.resolve<IFactory<interface1()>>();
    auto product = factory();
    static_assert(std::is_same_v<decltype(product), std::unique_ptr<interface1>>);
    assert(dynamic_cast<impl1_without_deps*>(product.get()));
  }

  injector.bind<IFactory<interface1()>>().to<Factory<impl1_with_deps(interface2&)>>();
  {
    auto factory = injector.resolve<IFactory<interface1()>>();
    auto product = factory();
    static_assert(std::is_same_v<decltype(product), std::unique_ptr<interface1>>);
    assert(dynamic_cast<impl1_with_deps*>(product.get()));
  }
};

Test test_factory_with_args_resolved_types = [] {
  auto injector = Injector<IFactory<interface1(int)>, SharedResolver<interface2>>();
  injector.bind<IFactory<interface1(int)>>().to<Factory<impl1_without_deps(), int>>();
  {
    auto factory = injector.resolve<IFactory<interface1(int)>>();
    auto product = factory(42);
    static_assert(std::is_same_v<decltype(product), std::unique_ptr<interface1>>);
    assert(dynamic_cast<impl1_without_deps*>(product.get()));
    assert(dynamic_cast<impl1_without_deps*>(product.get())->value == 42);
  }

  injector.bind<IFactory<interface1(int)>>().to<Factory<impl1_with_deps(interface2&), int>>();
  {
    auto factory = injector.resolve<IFactory<interface1(int)>>();
    auto product = factory(42);
    static_assert(std::is_same_v<decltype(product), std::unique_ptr<interface1>>);
    assert(dynamic_cast<impl1_with_deps*>(product.get()));
    assert(dynamic_cast<impl1_with_deps*>(product.get())->value == 42);
  }
};

int main() {

}