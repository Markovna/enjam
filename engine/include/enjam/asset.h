#ifndef INCLUDE_ENJAM_ASSET_H_
#define INCLUDE_ENJAM_ASSET_H_

#include <string>
#include <vector>
#include <variant>
#include <enjam/byte_array.h>
#include <enjam/log.h>
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

  using array_iterator = std::conditional_t<std::is_const_v<TAsset>,
      typename TAsset::array_t::const_iterator,
      typename TAsset::array_t::iterator>;

  using object_iterator = std::conditional_t<std::is_const_v<TAsset>,
      typename TAsset::object_t::const_iterator,
      typename TAsset::object_t::iterator>;

  using key_type = std::string;

  AssetIterator(array_iterator it) noexcept
      : it(it) {}

  AssetIterator(object_iterator it) noexcept
      : it(it) {}

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
    if(auto arrayIt = std::get_if<array_iterator>(&it); arrayIt) { return *std::next(*arrayIt, n); }
    throw std::runtime_error("Cannot use operator[] for object iterators");
  }

  const key_type& key() const {
    if(auto objectIt = std::get_if<object_iterator>(&it); objectIt) { return (*objectIt)->name; }
    throw std::runtime_error("Cannot access key in array iterators");
  }

 private:
  using iterator = std::variant<array_iterator, object_iterator>;

  iterator it;
};

template<class T>
struct AssetParser;

template<typename T, typename Enable = void>
struct has_from_asset : std::false_type {};

template<typename T>
struct has_from_asset<
    T, std::void_t<decltype(AssetParser<T>::fromAsset(std::declval<const Asset&>(), std::declval<T&>()))>>
    : std::true_type {
};

template<typename T, typename Enable = void>
struct has_to_asset : std::false_type {};

template<typename T>
struct has_to_asset<
    T, std::void_t<decltype(AssetParser<T>::toAsset(std::declval<Asset&>(), std::declval<const T&>()))>>
    : std::true_type {
};

enum class AssetType {
  INTEGER,
  FLOAT,
  STRING,
  OBJECT,
  ARRAY,
  BUFFER
};

using AssetBufferLoader = std::function<ByteArray()>;

class Asset final {
  using Buffer = ByteArray;
  using int_t = int64_t;
  using float_t = float;
  using string_t = std::string;
  using object_t = std::vector<Property>;
  using array_t = std::vector<Asset>;
  using buffer_loader_t = AssetBufferLoader;

  using value_t = std::variant<int_t, float_t, string_t, object_t, array_t, buffer_loader_t>;

  template<AssetType Type>
  static constexpr size_t variantIndex = static_cast<size_t>(Type);

  template<AssetType Type>
  using variant_alternative_t = std::variant_alternative_t<variantIndex<Type>, value_t>;

  template<AssetType TAssetType>
  struct type_identity { using type = variant_alternative_t<TAssetType>; };

  static_assert(std::is_same_v<int_t, variant_alternative_t<AssetType::INTEGER>>);
  static_assert(std::is_same_v<float_t, variant_alternative_t<AssetType::FLOAT>>);
  static_assert(std::is_same_v<string_t, variant_alternative_t<AssetType::STRING>>);
  static_assert(std::is_same_v<object_t, variant_alternative_t<AssetType::OBJECT>>);
  static_assert(std::is_same_v<array_t, variant_alternative_t<AssetType::ARRAY>>);
  static_assert(std::is_same_v<buffer_loader_t, variant_alternative_t<AssetType::BUFFER>>);

  friend class AssetIterator<Asset>;
  friend class AssetIterator<const Asset>;

 public:
  using iterator = AssetIterator<Asset>;
  using const_iterator = AssetIterator<const Asset>;

 public:
  Asset() = default;
  Asset(const Asset&) = default;
  Asset(Asset&&) = default;

  static Asset array() { return Asset(type_identity<AssetType::ARRAY>{}); }
  static Asset object() { return Asset(type_identity<AssetType::OBJECT>{}); }

  template<class T, std::enable_if_t<std::is_integral<T>::value, bool> = true>
  Asset(T val) : value((int_t) val) {}

  template<class T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
  Asset(T val) : value((float_t) val) {}

  template<class T, std::enable_if_t<std::is_constructible<string_t, T>::value, bool> = true>
  Asset(T&& val) : value(std::forward<T>(val)) {}

  template<class T, std::enable_if_t<std::is_invocable_r<Buffer, T>::value, bool> = true>
  Asset(T val) : value(val) {}

  template<class T, std::enable_if_t<std::is_assignable<Buffer, T>::value, bool> = true>
  Asset(T&& val) : value([buf = std::forward<T>(val)] { return buf; }) {}

  template<class T, std::enable_if_t<std::is_integral<T>::value, bool> = true>
  Asset& operator=(T);

  template<class T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
  Asset& operator=(T);

  template<class T, std::enable_if_t<has_to_asset<T>::value, bool> = true>
  Asset& operator=(const T&);

  template<class T, std::enable_if_t<std::is_constructible<string_t, T>::value, bool> = true>
  Asset& operator=(T&&);

  template<class T, std::enable_if_t<std::is_invocable<T>::value, bool> = true>
  Asset& operator=(T&&);

  Asset& operator=(const Asset&) = default;
  Asset& operator=(Asset&&) noexcept = default;

  Asset& operator[](const std::string_view&);
  Asset& operator[](size_t);

  template<class T, std::enable_if_t<std::is_arithmetic<T>::value, bool> = true>
  T as() const;

  template<class T, std::enable_if_t<has_from_asset<T>::value, bool> = true>
  T as() const;

  template<class T, std::enable_if_t<std::is_same<T, string_t>::value, bool> = true>
  const T& as() const;

  template<AssetType TYPE>
  constexpr inline bool is() const { return value.index() == variantIndex<TYPE>; }

  constexpr inline AssetType type() const { return static_cast<AssetType>(value.index()); }

  bool isNumeric() const;

  const Asset* at(const std::string&) const;

  void pushBack(const Asset&);
  void pushBack(Asset&&);

  const AssetBufferLoader& getBufferLoader() const { return std::get<buffer_loader_t>(value); }
  Buffer loadBuffer() const {
    if(!holds<buffer_loader_t>()) {
      ENJAM_ERROR("Failed to load buffer because asset doesn't hold a buffer");
      return { };
    }

    return std::get<buffer_loader_t>(value)();
  }
  void clear() { value = {}; }

  const_iterator begin() const {
    return cbegin();
  }

  const_iterator end() const {
    return cend();
  }

  iterator begin() {
    if(auto iterable = std::get_if<object_t>(&value); iterable) { return { iterable->begin() }; }
    if(auto iterable = std::get_if<array_t>(&value); iterable) { return { iterable->begin() }; }

    throw std::runtime_error("Can not get iterator of not iterable type");
  }

  iterator end() {
    if(auto iterable = std::get_if<object_t>(&value); iterable) { return { iterable->end() }; }
    if(auto iterable = std::get_if<array_t>(&value); iterable) { return { iterable->end() }; }

    throw std::runtime_error("Can not get iterator of not iterable type");
  }

  const_iterator cbegin() const {
    if(auto iterable = std::get_if<object_t>(&value); iterable) { return { iterable->begin() }; }
    if(auto iterable = std::get_if<array_t>(&value); iterable) { return { iterable->begin() }; }

    throw std::runtime_error("Can not get iterator of not iterable type");
  }

  const_iterator cend() const {
    if(auto iterable = std::get_if<object_t>(&value); iterable) { return { iterable->end() }; }
    if(auto iterable = std::get_if<array_t>(&value); iterable) { return { iterable->end() }; }

    throw std::runtime_error("Can not get iterator of not iterable type");
  }

 private:
  template<AssetType TAssetType, class... Args, std::enable_if_t<std::is_constructible_v<variant_alternative_t<TAssetType>, Args...>, bool> = true>
  Asset(type_identity<TAssetType>, Args&&... args) : value(std::in_place_type<variant_alternative_t<TAssetType>>, std::forward<Args>(args)...) { }

  template<class T>
  T& ensureType() {
    if (!std::holds_alternative<T>(value)) {
      value = T {};
    }

    return std::get<T>(value);
  }

  template<class T>
  inline bool holds() const { return std::holds_alternative<T>(value); }

 private:
  value_t value;
};

struct Property final {
  using KeyType = std::string;
  KeyType name;
  uint64_t nameHash;
  Asset value;
};

template<class T, std::enable_if_t<std::is_arithmetic<T>::value, bool>>
T Asset::as() const {
  auto t = type();
  switch (t) {
    case AssetType::FLOAT: { return (T) std::get<float_t>(value); }
    case AssetType::INTEGER: { return (T) std::get<int_t>(value); }
    default: break;
  }
  return 0;
}

template<class T, std::enable_if_t<std::is_same<T, Asset::string_t>::value, bool>>
const T& Asset::as() const {
  return std::get<string_t>(value);
}

template<class T, std::enable_if_t<has_from_asset<T>::value, bool>>
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

template<class T, std::enable_if_t<has_to_asset<T>::value, bool>>
Asset& Asset::operator=(const T& arg) {
  AssetParser<T>::toAsset(*this, arg);
  return *this;
}

template<class T, std::enable_if_t<std::is_constructible<Asset::string_t, T>::value, bool>>
Asset& Asset::operator=(T&& arg) {
  value = std::forward<T>(arg);
  return *this;
}

template<class T, std::enable_if_t<std::is_invocable<T>::value, bool>>
Asset& Asset::operator=(T&& func) {
  value = std::forward<T>(func);
  return *this;
}

}

#endif //INCLUDE_ENJAM_ASSET_H_
