#ifndef INCLUDE_ENJECTOR_FACTORIES_H_
#define INCLUDE_ENJECTOR_FACTORIES_H_

#include <functional>

namespace enjector {

template<class TProduct, class... TArgs>
using IFactory = std::function<std::unique_ptr<TProduct>(TArgs...)>;

template<class...>
class Factory;

template<class TProduct, class... TArgs, class... TDeps>
class Factory<TProduct(TArgs...), TDeps...> {
 public:
  explicit Factory(TDeps... deps) : deps(std::forward<TDeps>(deps)...) {
  }

  std::unique_ptr<TProduct> operator()(TArgs... args) const {
    return std::make_unique<TProduct>(std::get<TDeps>(deps)..., std::forward<TArgs>(args)...);
  }

 private:
  std::tuple<TDeps...> deps;
};

template<class TProduct, class TDependencies, class... TArgs>
class Binding<IFactory<TProduct, TArgs...>, TDependencies> {
 public:
  using Dependency = Dependency<IFactory<TProduct, TArgs...>, TDependencies>;
  using FactoryInterface = IFactory<TProduct, TArgs...>;

  explicit Binding(Dependency& dependency) : dependency(dependency) {}

  template<class TProductImpl, class... TDeps>
  void to() {
    dependency.assign(UniqueConstructor<TProductImpl, TDeps...>{});
  }

 private:
  template<class...>
  struct UniqueConstructor;

  template<class TProductImpl, class... TDeps>
  struct UniqueConstructor<Factory<TProductImpl(TArgs...), TDeps...>> {
    std::unique_ptr<FactoryInterface> operator()(TDependencies *dependencies) const {
      return std::make_unique<FactoryInterface>(Factory<TProductImpl(TArgs...), TDeps...> { dependencies->template resolve<TDeps>()... });
    }
  };

 private:
  Dependency &dependency;
};


}

#endif //INCLUDE_ENJECTOR_FACTORIES_H_
