#ifndef INCLUDE_ENJAM_ASSET_H_
#define INCLUDE_ENJAM_ASSET_H_

#include <string>
#include <vector>
#include <variant>
#include <enjam/asset_buffer.h>
#include <enjam/type_traits_helpers.h>

namespace Enjam {

class Property;
class Asset;

template<class TAsset>
class AssetIterator {
 public:
  using difference_type = std::ptrdiff_t;
  using value_type = TAsset;
  using reference = typename std::add_lvalue_reference<TAsset>::type;
  using pointer = typename std::add_pointer<TAsset>::type;
  using iterator_category = std::bidirectional_iterator_tag;

  using array_iterator = typename TAsset::array_t::iterator;
  using object_iterator = typename TAsset::object_t::iterator;

  AssetIterator(array_iterator it) noexcept
      : it(it) { }

  AssetIterator(object_iterator it) noexcept
      : it(it) { }

  reference operator*() const {
    return std::visit(overloaded {
        [](const array_iterator& arg) -> reference { return *arg; },
        [](const object_iterator& arg) -> reference { return (*arg).value; }
    }, it);
  }

  pointer operator->() const {
    return std::visit(overloaded {
        [](const array_iterator& arg) { return &(*arg); },
        [](const object_iterator& arg) { return &(arg->value); }
    }, it);
  }

  AssetIterator operator++(int)& {
    auto result = *this;
    ++(*this);
    return result;
  }

  AssetIterator& operator++() {
    std::visit([](auto& arg) { std::advance(arg, 1); }, it);
    return *this;
  }

  AssetIterator operator--(int)& {
    auto result = *this;
    --(*this);
    return result;
  }

  AssetIterator& operator--() {
    std::visit([](auto& arg) { std::advance(arg, -1); }, it);
    return *this;
  }

  template<typename TIter, std::enable_if_t<std::is_same<TIter, AssetIterator>::value, std::nullptr_t> = nullptr>
  bool operator==(const TIter& other) const {
    return std::visit(overloaded {
        [&other](const array_iterator& arg) { return std::get<array_iterator>(other.it) == arg; },
        [&other](const object_iterator& arg) { return std::get<object_iterator>(other.it) == arg; }
        }, it);
  }

  template<typename TIter, std::enable_if_t<std::is_same<TIter, AssetIterator>::value, std::nullptr_t> = nullptr>
  bool operator!=(const TIter& other) const {
    return !operator==(other);
  }

  reference operator[](difference_type n) const {
    return std::visit(overloaded {
        [&n](const array_iterator& arg) { return *std::next(arg, n); },
        [](const object_iterator& arg) { throw std::invalid_argument("Cannot use operator[] for object iterators"); }
    }, it);
  }

 private:
  using iterator = std::variant<array_iterator, object_iterator>;

  iterator it;
};

template<class T>
struct AssetParser;

template <typename T, typename Enable = void>
struct has_asset_parser : std::false_type {};

template <typename T>
struct has_asset_parser<
    T, std::void_t<decltype(AssetParser<T>::fromAsset(std::declval<const Asset&>(), std::declval<T&>()))>>
    : std::true_type {};

class Asset final {
 public:
  using int_t = int64_t;
  using float_t = float;
  using string_t = std::string;
  using object_t = std::vector<Property>;
  using array_t = std::vector<Asset>;
  using buffer_t = std::function<AssetBuffer()>;
  using value_t = std::variant<int_t, float_t, string_t, object_t, array_t, buffer_t>;

 public:
  Asset() = default;
  Asset(const Asset&) = default;
  Asset(Asset&&) = default;

  template<class T, std::enable_if_t<std::is_integral<T>::value, bool> = true>
  Asset(T val) : value((int_t) val) { }

  template<class T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
  Asset(T val) : value((float_t) val) { }

  template<class T, std::enable_if_t<std::is_constructible<string_t, T>::value, bool> = true>
  Asset(T&& val) : value(std::forward<T>(val)) { }

  template<class T, std::enable_if_t<std::is_invocable_r<AssetBuffer, T>::value, bool> = true>
  Asset(T val) : value(val) { }

  template<class T, std::enable_if_t<std::is_assignable<AssetBuffer, T>::value, bool> = true>
  Asset(T&& val) : value([buf = std::forward<T>(val)] { return buf; }) { }

  template<class T, std::enable_if_t<std::is_integral<T>::value, bool> = true>
  Asset& operator=(T);

  template<class T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
  Asset& operator=(T);

  template<class T, std::enable_if_t<std::is_constructible<string_t, T>::value, bool> = true>
  Asset& operator=(T&&);

  template<class T, std::enable_if_t<std::is_invocable<T>::value, bool> = true>
  Asset& operator=(T&&);

  Asset& operator=(const Asset&) = default;
  Asset& operator=(Asset&&) noexcept = default;

  Asset& operator[](const std::string_view&);
  Asset& operator[](size_t);

  template<class T, std::enable_if_t<std::is_integral<T>::value, bool> = true>
  T as() const;

  template<class T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
  T as() const;

  template<class T, std::enable_if_t<std::is_same<T, string_t>::value, bool> = true>
  const T& as() const;

  template<class T, std::enable_if_t<has_asset_parser<T>::value, bool> = true>
  T as() const;

  template<class T>
  bool is() const { return std::holds_alternative<T>(value); }

  bool isNumeric() const;

  template<class TVisitor>
  decltype(auto) visit(TVisitor&& visitor) const {
    return std::visit(std::forward<TVisitor>(visitor), value);
  }

  const Asset* at(const std::string&) const;

  void pushBack(const Asset&);
  void pushBack(Asset&&);

  AssetBuffer load() const { return std::get<buffer_t>(value)(); }
  void clear() { value = {}; }

  AssetIterator<const Asset> begin() const {
    return cbegin();
  }

  AssetIterator<const Asset> end() const {
    return cend();
  }

  AssetIterator<Asset> begin() {
    using Iterator = AssetIterator<Asset>;
    return std::visit(overloaded {
        [](object_t& obj) -> Iterator { return { obj.begin() }; },
        [](array_t& obj) -> Iterator { return { obj.begin() }; },
        [](auto&) -> Iterator { throw std::invalid_argument("Can not get iterator of not iteratable type"); }
    }, value);
  }

  AssetIterator<Asset> end() {
    using Iterator = AssetIterator<Asset>;
    return std::visit(overloaded {
        [](object_t& obj) -> Iterator { return { obj.end() }; },
        [](array_t& obj) -> Iterator { return { obj.end() }; },
        [](auto&) -> Iterator { throw std::invalid_argument("Can not get iterator of not iteratable type"); }
    }, value);
  }

  AssetIterator<const Asset> cbegin() const {
    using Iterator = AssetIterator<const Asset>;
    return std::visit(overloaded {
        [](object_t& obj) -> Iterator { return { obj.begin() }; },
        [](array_t& obj) -> Iterator { return { obj.begin() }; },
        [](auto&) -> Iterator { throw std::invalid_argument("Can not get iterator of not iteratable type"); }
    }, value);
  }

  AssetIterator<const Asset> cend() const {
    using Iterator = AssetIterator<const Asset>;
    return std::visit(overloaded {
        [](object_t& obj) -> Iterator { return { obj.end() }; },
        [](array_t& obj) -> Iterator { return { obj.end() }; },
        [](auto&) -> Iterator { throw std::invalid_argument("Can not get iterator of not iteratable type"); }
    }, value);
  }

 private:
  template<class T>
  T& ensureType() {
    if(!std::holds_alternative<T>(value)) {
      value = T { };
    }

    return std::get<T>(value);
  }

 private:
  value_t value;
};

struct Property final {
  using KeyType = std::string;
  KeyType name;
  uint64_t nameHash;
  Asset value;
};

template<class T, std::enable_if_t<std::is_integral<T>::value, bool>>
T Asset::as() const {
  return (T) std::get<int_t>(value);
}

template<class T, std::enable_if_t<std::is_floating_point<T>::value, bool>>
T Asset::as() const {
  return (T) std::get<float_t>(value);
}

template<class T, std::enable_if_t<std::is_same<T, Asset::string_t>::value, bool>>
const T& Asset::as() const {
  return std::get<string_t>(value);
}

template<class T, std::enable_if_t<has_asset_parser<T>::value, bool>>
T Asset::as() const {
  T val;
  AssetParser<T>::fromAsset(*this, val);
  return val;
}

template<class T, std::enable_if_t<std::is_integral<T>::value, bool>>
Asset& Asset::operator=(T arg) {
  value = (int_t) arg;
  return *this;
}

template<class T, std::enable_if_t<std::is_floating_point<T>::value, bool>>
Asset& Asset::operator=(T arg) {
  value = (float_t) arg;
  return *this;
}

template<class T, std::enable_if_t<std::is_constructible<Asset::string_t, T>::value, bool>>
Asset& Asset::operator=(T&& arg) {
  value = std::forward<T>(arg);
  return *this;
}

template<class T, std::enable_if_t<std::is_invocable<T>::value, bool>>
Asset& Asset::operator=(T&& func) {
  value = func;
  return *this;
}

}

#endif //INCLUDE_ENJAM_ASSET_H_
