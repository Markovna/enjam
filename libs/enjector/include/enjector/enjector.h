#ifndef INCLUDE_ENJECTOR_DEPENDENCIES_H_
#define INCLUDE_ENJECTOR_DEPENDENCIES_H_

#include <functional>

namespace enjector {

template<class T, class Enable = void>
struct DefaultUniqueConstructor {
  template<class TDependencies>
  std::unique_ptr<T> operator()(TDependencies*) {
    return std::unique_ptr<T>{};
  }
};

template<class T>
struct DefaultUniqueConstructor<T, std::enable_if_t<std::is_default_constructible_v<T>>> {
  template<class TDependencies>
  std::unique_ptr<T> operator()(TDependencies*) {
    return std::make_unique<T>();
  }
};

template<class T>
struct type {};

template<class T, class TDependencies>
class Dependency {
 public:
  using ReferenceType = std::add_lvalue_reference_t<T>;

  class Reference {
   public:
    explicit Reference(ReferenceType ref) : ref(ref) {}

    T *resolve(TDependencies *) { return &ref; }

   private:
    ReferenceType ref;
  };

  class Unique {
   public:
    using ConstructorCallback = std::function<std::unique_ptr<T>(TDependencies *)>;

    explicit Unique(ConstructorCallback &&ctr) : ptr(), constructor(std::move(ctr)) {}
    Unique() : ptr(), constructor(DefaultUniqueConstructor<T>{}) {}

    T *resolve(TDependencies *dependencies) {
      if (!ptr) {
        ptr = constructor(dependencies);
      }
      return ptr.get();
    }

   private:
    std::unique_ptr<T> ptr;
    ConstructorCallback constructor;
  };

  Dependency() = default;
  Dependency(Dependency&&) noexcept = default;

  T* resolve(TDependencies* dependencies) {
    return std::visit([dependencies](auto &arg) { return arg.resolve(dependencies); }, value);
  }

  template<class Callable, std::enable_if_t<std::is_convertible_v<Callable, typename Unique::ConstructorCallback>, bool> = true>
  void assign(Callable &&constructor) {
    value.template emplace<Unique>(std::forward<Callable>(constructor));
  }

  void assign(ReferenceType val) {
    value.template emplace<Reference>(val);
  }

  void clear() {
    value = Unique();
  }

 private:
  std::variant<Unique, Reference> value = {};
};

template<class T, class TDependencies>
class Binding {
 public:
  using Dependency = Dependency<T, TDependencies>;

  explicit Binding(Dependency& dependency) : dependency(dependency) {}

  void to(T &ref) {
    dependency.assign(ref);
  }

  template<class D>
  void to() {
    dependency.assign(UniqueConstructor<D>{});
  }

 private:
  template<class D>
  struct UniqueConstructor {
    std::unique_ptr<T> operator()(TDependencies*) const {
      return std::make_unique<D>();
    }
  };

  template<class R, class... Args>
  struct UniqueConstructor<R(Args...)> {
    std::unique_ptr<T> operator()(TDependencies *dependencies) const {
      return std::make_unique<R>(dependencies->template resolve<Args>()...);
    }
  };

 private:
  Dependency &dependency;
};

template<class... TDeps>
class Dependencies : Dependency<TDeps, Dependencies<TDeps...>>... {
 public:
  template<class T> using Dependency = Dependency<T, Dependencies>;
  template<class T> using Binding = Binding<T, Dependencies>;

  Dependencies() = default;
  Dependencies(Dependencies&&) noexcept = default;

  template<class T>
  auto bind() { return Binding<T> { get<T>() }; }

 public:
  template<class T>
  T resolve() {
    return resolve(type<T>{});
  }

  void clear() {
    (get<TDeps>().clear(), ...);
  }

 private:
  template<class T>
  T& resolve(type<T&>) {
    return *get<T>().resolve(this);
  }

  template<class T>
  T* resolve(type<T*>) {
    return get<T>().resolve(this);
  }

  template<class T, std::enable_if_t<std::is_base_of_v<Dependency<T>, Dependencies>, bool> = true>
  Dependency<T>& get() {
    return static_cast<Dependency<T>&>(*this);
  }
};

}

#endif //INCLUDE_ENJECTOR_DEPENDENCIES_H_
