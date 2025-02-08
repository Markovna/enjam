#ifndef INCLUDE_ENJAM_HANDLE_ALLOCATOR_H_
#define INCLUDE_ENJAM_HANDLE_ALLOCATOR_H_

#include <tuple>
#include <vector>

namespace Enjam {

template<typename T>
class Handle {
 public:
  using HandleId = uint32_t;
  static constexpr HandleId invalidId = HandleId { UINT32_MAX };

  Handle() noexcept = default;

  Handle(const Handle&) noexcept = default;
  Handle& operator=(const Handle&) noexcept = default;

  Handle(Handle&& rhs) noexcept
      : id(rhs.id) {
    rhs.id = invalidId;
  }

  explicit Handle(HandleId id) noexcept : id(id) { }

  Handle& operator=(Handle&& rhs) noexcept {
    if(this != &rhs) {
      id = rhs.id;
      rhs.id = invalidId;
    }
    return *this;
  }

  bool operator ==(Handle other) const noexcept { return id == other.id; }
  bool operator !=(Handle other) const noexcept { return id != other.id; }

  explicit constexpr operator bool() const noexcept {return id != invalidId; }

  template<typename D, typename = std::enable_if_t<std::is_base_of<T, D>::value> >
  Handle(const Handle<D>& derived) noexcept : Handle(derived.id) { }

  HandleId getId() const noexcept { return id; }

 private:

  template<class U> friend class Handle;

 private:
  HandleId id = invalidId;
};

template<class ...TYPES>
class HandleAllocator final {
 private:
  static constexpr uint8_t NODE_FREE_TAG = 0x01;

  template<class T>
  struct Node {
    T value;
    uint8_t tags;

    Node() = delete;
    explicit Node(T&& value) : value(std::move(value)), tags(0) { }
  };

  template<class T> using Pool = std::vector<Node<T>>;

 public:
  // TODO
  template<class D> Handle<D> alloc();
  template<class D, class ...ARGS> void construct(const Handle<D>&, ARGS&& ...);
  template<class D> void destroy(Handle<D>&);

  template<class D> void dealloc(Handle<D>& handle) {
    auto& pool = getPool<D>();
    auto& node = pool[handle.getId()];
    node.tags |= NODE_FREE_TAG;
  }

  template<class B, class D, typename = typename std::enable_if_t<std::is_base_of_v<B, D>, D>>
  void dealloc(Handle<B>& handle, const D* p) {
    auto& pool = getPool<D>();
    auto& node = pool[handle.getId()];
    node.tags |= NODE_FREE_TAG;
  }

  template<class D, class ...ARGS> Handle<D> allocAndConstruct(ARGS&&... args) {
    auto& pool = getPool<D>();
    auto first = std::find_if(pool.begin(), pool.end(), [](auto& node) { return node.tags & NODE_FREE_TAG; });
    if(first != pool.end()) {
      first->value = D(std::forward<ARGS>(args)...);
      first->tags ^= NODE_FREE_TAG;
      auto idx = std::distance(pool.begin(), first);
      return Handle<D> { static_cast<typename Handle<D>::HandleId>(idx) };
    }

    auto idx = pool.size();
    pool.emplace_back(D(std::forward<ARGS>(args)...));
    return Handle<D> { static_cast<typename Handle<D>::HandleId>(idx) };
  }

  template<typename Dp, typename B>
  typename std::enable_if_t<
      std::is_pointer_v<Dp> &&
          std::is_base_of_v<B, typename std::remove_pointer_t<Dp>>, Dp>
  cast(Handle<B>& handle) {
    using D = std::remove_pointer_t<Dp>;
    auto& pool = getPool<D>();
    auto& node = pool[handle.getId()];
    return &node.value;
  }

 private:
  template<class T> Pool<T>& getPool() {
    return std::get<Pool<T>>(pools);
  }

 private:
  std::tuple<Pool<TYPES>...> pools {};
};

}

#endif // INCLUDE_ENJAM_HANDLE_ALLOCATOR_H_
