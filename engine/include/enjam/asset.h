#ifndef INCLUDE_ENJAM_ASSET_H_
#define INCLUDE_ENJAM_ASSET_H_

#include <vector>
#include <string>

namespace Enjam {

class Property;
class Asset;

struct BufferHash {
  std::string value;
};


class Asset final {
 public:
  using int_t = int64_t;
  using float_t = float;
  using string_t = std::string;
  using object_t = std::vector<Property>;
  using array_t = std::vector<Asset>;
  using value_t = std::variant<int_t, float_t, string_t, object_t, array_t, BufferHash>;

 public:
  template<class T, std::enable_if_t<std::is_integral<T>::value, bool> = true>
  T as() const;

  template<class T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
  T as() const;

  template<class T, std::enable_if_t<std::is_same<T, string_t>::value, bool> = true>
  const T& as() const;

  template<class T>
  bool is() const { return std::holds_alternative<T>(value); }

  Asset() = default;
  Asset(const Asset&) = default;
  Asset(Asset&&) = default;

  template<class T, std::enable_if_t<std::is_integral<T>::value, bool> = true>
  Asset& operator=(T);

  template<class T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
  Asset& operator=(T);

  template<class T, std::enable_if_t<std::is_same<T, string_t>::value, bool> = true>
  Asset& operator=(T&&);

  Asset& operator=(const Asset&) = default;
  Asset& operator=(Asset&&) noexcept = default;

  Asset& operator[](const std::string&);
  Asset& operator[](size_t);

  const Asset* at(const std::string&) const;
  void pushBack(const Asset&);
  void pushBack(Asset&&);

  void clear() { value = {}; }

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

template<class T, std::enable_if_t<std::is_same<T, std::string>::value, bool>>
Asset& Asset::operator=(T&& arg) {
  value = std::forward<T>(arg);
  return *this;
}

}

#endif //INCLUDE_ENJAM_ASSET_H_
