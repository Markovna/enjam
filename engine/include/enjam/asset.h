#ifndef INCLUDE_ENJAM_ASSETS_MANAGER_H_
#define INCLUDE_ENJAM_ASSETS_MANAGER_H_

#include <memory>
#include <utility>
#include <vector>
#include <sstream>
#include <unordered_map>
#include <enjam/utils.h>
#include <enjam/type_traits_helpers.h>
#include <enjam/log.h>

namespace Enjam {

class Property;
class Asset;

struct BufferHash {
  std::string value;
};


class Asset final {
 public:
  using object_t = std::vector<Property>;
  using array_t = std::vector<Asset>;
  using value_t = std::variant<int64_t, float_t, std::string, object_t, array_t, BufferHash>;


 public:
  template<class T, std::enable_if_t<std::is_integral<T>::value, bool> = true>
  T as() const;

  template<class T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
  T as() const;

  template<class T, std::enable_if_t<std::is_same<T, std::string>::value, bool> = true>
  const T& as() const;

  template<class T>
  bool is() const { return std::holds_alternative<T>(value); }

  Asset() = default;
  Asset(const Asset&) = default;
  Asset(Asset&&) = default;

  template<class T, std::enable_if_t<std::is_integral<std::remove_reference_t<T>>::value, bool> = true>
  Asset& operator=(T);

  template<class T, std::enable_if_t<std::is_floating_point<std::remove_reference_t<T>>::value, bool> = true>
  Asset& operator=(T);

  template<class T, std::enable_if_t<std::is_same<std::remove_reference_t<T>, std::string>::value, bool> = true>
  Asset& operator=(T&&);

  Asset& operator=(const Asset&) = default;
  Asset& operator=(Asset&&) noexcept = default;

  Asset& operator[](const std::string&);
  Asset& operator[](size_t);

  const Asset* at(const std::string&) const;

  void clear() { value = {}; }

 private:
  value_t value;
};

struct Property final {
  uint64_t nameHash;
  Asset value;
};

template<class T, std::enable_if_t<std::is_integral<T>::value, bool>>
T Asset::as() const {
  return (T) std::get<int64_t>(value);
}

template<class T, std::enable_if_t<std::is_floating_point<T>::value, bool>>
T Asset::as() const {
  return (T) std::get<float_t>(value);
}

template<class T, std::enable_if_t<std::is_same<T, std::string>::value, bool>>
const T& Asset::as() const {
  return std::get<std::string>(value);
}

template<class T, std::enable_if_t<std::is_integral<std::remove_reference_t<T>>::value, bool>>
Asset& Asset::operator=(T arg) {
  value = (int64_t) arg;
  return *this;
}

template<class T, std::enable_if_t<std::is_floating_point<std::remove_reference_t<T>>::value, bool>>
Asset& Asset::operator=(T arg) {
  value = (float_t) arg;
  return *this;
}

template<class T, std::enable_if_t<std::is_same<std::remove_reference_t<T>, std::string>::value, bool>>
Asset& Asset::operator=(T&& arg) {
  value = std::forward<T>(arg);
  return *this;
}

}

#endif //INCLUDE_ENJAM_ASSETS_MANAGER_H_
