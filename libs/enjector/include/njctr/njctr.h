#ifndef INCLUDE_ENJECTOR_DEPENDENCIES_H_
#define INCLUDE_ENJECTOR_DEPENDENCIES_H_

#include <functional>

namespace njctr {

template<class TResolver, class TInjector, class TExpected, class = void>
struct HasResolve : std::false_type {
};

template<class TResolver, class TInjector, class TExpected>
struct HasResolve<TResolver, TInjector, TExpected,
    std::void_t<decltype(std::declval<TResolver>().template resolve<TExpected>(std::declval<TInjector*>()))>>
  : std::true_type { };

template<class T, class = void>
struct HasBindType : std::false_type {

};

template<class T>
struct HasBindType<T, typename std::void_t<typename T::BindType>> : std::true_type {};

template<class TBindType, class TResolver, class = void>
struct IsBindable : std::false_type { };

template<class TBindType, class TResolver>
struct IsBindable<TBindType, TResolver, std::enable_if_t<HasBindType<TResolver>::value, void>>
    : std::integral_constant<bool, std::is_same_v<TBindType, typename TResolver::BindType>>
{ };

template<class TResolver>
struct ResolverTraits {
  template<class T, class TInjector>
  using IsResolvable = HasResolve<TResolver, TInjector, T>;

  template<class T>
  using IsBindable = IsBindable<T, TResolver>;

  template<class T, class TInjector>
  static decltype(auto) resolve(TResolver& resolver, TInjector* injector) {
    return resolver.template resolve<T>(injector);
  }

  template<class TInjector>
  static decltype(auto) bind(TResolver& resolver, TInjector* injector) {
    return resolver.template bind(injector);
  }

  static void reset(TResolver& resolver) {
    return resolver.reset();
  }
};

template<class TInjector, class ...TResolvers>
struct InjectorTraits {
  template<class T>
  using AllResolvers = decltype(std::tuple_cat(
      std::declval<std::conditional_t<ResolverTraits<TResolvers>::template IsResolvable<T, TInjector>::value,
                                      std::tuple<TResolvers>,
                                      std::tuple<>>>()...));

  template<class T>
  using AllBindable = decltype(std::tuple_cat(
      std::declval<std::conditional_t<ResolverTraits<TResolvers>::template IsBindable<T>::value,
                                      std::tuple<TResolvers>,
                                      std::tuple<>>>()...));

  template<class T>
  using FirstResolver = typename std::tuple_element<0, AllResolvers<T>>::type;

  template<class T>
  using FirstBindable = typename std::tuple_element<0, AllBindable<T>>::type;

  template<class T>
  using HasAnyResolver = std::integral_constant<bool, std::tuple_size<AllResolvers<T>>::value>;

  template<class T>
  using HasAnyBindable = std::integral_constant<bool, std::tuple_size<AllBindable<T>>::value>;
};

template<class... TResolvers>
class Injector {
 public:
  using Traits = InjectorTraits<Injector, TResolvers...>;

  template<class T>
  using Resolver = typename Traits::template FirstResolver<T>;

  template<class T>
  using Bindable = typename Traits::template FirstBindable<T>;

  template<class T, std::enable_if_t<Traits::template HasAnyResolver<T>::value, bool> = true>
  decltype(auto) resolve() {
    using Resolver = Resolver<T>;
    auto& resolver = get<Resolver>();
    return ResolverTraits<Resolver>::template resolve<T>(resolver, this);
  }

  template<class T, std::enable_if_t<Traits::template HasAnyBindable<T>::value, bool> = true>
  decltype(auto) bind() {
    using Bindable = Bindable<T>;
    auto& resolver = get<Bindable>();
    return ResolverTraits<Bindable>::bind(resolver, this);
  }

  void reset() {
    (ResolverTraits<TResolvers>::reset(get<TResolvers>()), ...);
  }

 private:
  using Pool = std::tuple<TResolvers...>;

  template<class TResolver>
  decltype(auto) get() { return std::get<TResolver>(resolvers); }

 private:
  Pool resolvers;
};

template<class TInjector, class T>
struct SharedConstructor {
  explicit SharedConstructor(TInjector&) {}
  auto operator()() const { return std::make_shared<T>(); }
};

template<class TInjector, class R, class... Args>
struct SharedConstructor<TInjector, R(Args...)> {
  explicit SharedConstructor(TInjector& injector) : injector(injector) {}
  auto operator()() const { return std::make_shared<R>(injector.template resolve<Args>()...); }

 private:
  TInjector& injector;
};

template<class T>
class SharedBinding {
 public:
  using ConstructorCallback = std::function<std::shared_ptr<T>()>;

  SharedBinding() : object(), ctr() {}

  explicit operator T &() { return *object; }
  explicit operator const T &() const { return *object; }
  explicit operator std::shared_ptr<T>() const { return object; }

  template<class TCtr, std::enable_if_t<std::is_convertible_v<TCtr, ConstructorCallback>, bool> = true>
  void bindTo(TCtr &&constructor) {
    ctr = std::forward<TCtr>(constructor);
    object.reset();
  }

  template<class D, std::enable_if_t<std::is_convertible_v<std::shared_ptr<D>, std::shared_ptr<T>>, bool> = true>
  void bindTo(std::shared_ptr<D> &&ptr) {
    ctr = {};
    object = std::move(ptr);
  }

  SharedBinding &resolve() {
    if (!object && ctr) { object = ctr(); }
    return *this;
  }

  void reset() {
    ctr = {};
    object.reset();
  }

 private:
  std::shared_ptr<T> object{};
  ConstructorCallback ctr{};
};

template<class T, class TInjector>
class SharedBinder {
 public:
  using Binding = SharedBinding<T>;

  SharedBinder(TInjector& injector, Binding &binding) : injector(injector), binding(binding) {}

  template<class TCtr>
  void to() {
    binding.bindTo(SharedConstructor<TInjector, TCtr>{injector});
  }

  template<class D>
  void to(std::shared_ptr<D> ptr) {
    binding.bindTo(std::move(ptr));
  }

 private:
  TInjector& injector;
  Binding &binding;
};

template<class T>
class SharedResolver {
 public:
  using BindType = T;

  template<class TExpected>
  using IsResolvable = std::disjunction<
      std::is_same<T&, TExpected>,
      std::is_same<const T&, TExpected>,
      std::is_same<std::shared_ptr<T>, TExpected>>;

  template<class TExpected, class TInjector, std::enable_if_t<IsResolvable<TExpected>::value, bool> = true>
  TExpected resolve(TInjector*) {
    return (TExpected) binding.resolve();
  }

  template<class TInjector>
  auto bind(TInjector* injector) { return SharedBinder { *injector, binding }; }

  void reset() { binding.reset(); }

 private:
  SharedBinding<T> binding;
};

template<class...>
struct Factory;

template<class TProduct, class... TArgs, class... TDeps>
struct Factory<TProduct(TDeps...), TArgs...> {
  template<class TInjector>
  std::unique_ptr<TProduct> operator()(TInjector& injector, TArgs&&... args) const {
    return std::make_unique<TProduct>(injector.template resolve<TDeps>()..., std::forward<TArgs>(args)...);
  }
};

template<class TInjector, class R, class...TArgs>
class FactoryBinder {
 public:
  using IFactory = std::function<std::unique_ptr<R>(TArgs...)>;

  FactoryBinder(TInjector& injector, IFactory& factory) : injector(injector), factory(factory) { }

  template<class TFactory>
  void to() {
    factory = [&injector = injector](TArgs&&...args) -> std::unique_ptr<R> {
      return TFactory{}.operator()(injector, std::forward<TArgs>(args)...);
    };
  }

 private:
  TInjector& injector;
  IFactory& factory;
};

template<class>
class IFactory;

template<class TProduct, class... TArgs>
class IFactory<TProduct(TArgs...)> {
 public:
  using Function = std::function<std::unique_ptr<TProduct>(TArgs...)>;
  using BindType = IFactory;

  template<class TExpected, class TInjector, std::enable_if_t<std::is_same_v<TExpected, IFactory>, bool> = true>
  Function resolve(TInjector*) { return function; }

  template<class TInjector>
  decltype(auto) bind(TInjector* injector) {
    return FactoryBinder { *injector, function };
  }

  void reset() { function = {}; }

 private:
  Function function;
};

}

#endif //INCLUDE_ENJECTOR_DEPENDENCIES_H_
