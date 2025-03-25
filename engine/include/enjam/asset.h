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

using value_t = std::variant<int64_t, float_t, std::string, Asset, BufferHash>;

class Asset final {
 public:
  template<class T, std::enable_if_t<is_same_with_any<T, int64_t, float_t, std::string>::value, bool> = true>
  T get(uint64_t hash) const;

  template<class T, std::enable_if_t<std::is_same_v<T, Asset>, bool> = true>
  const Asset* get(uint64_t hash) const;

  template<class T>
  bool isType(uint64_t hash) const;

  bool empty() const { return properties.empty(); }

  void add(Property&& prop) {
    properties.push_back(std::move(prop));
  }

  void clear() { properties.clear(); }

 private:
  std::vector<Property> properties;
};

struct Property final {
  uint64_t nameHash;
  value_t value;
};

template<class T>
bool Asset::isType(uint64_t hash) const {
  for (auto& property : properties) {
    if(property.nameHash == hash) {
      return std::holds_alternative<T>(property.value);
    }
  }
  return false;
}

template<class T, std::enable_if_t<is_same_with_any<T, int64_t, float_t, std::string>::value, bool>>
T Asset::get(uint64_t hash) const {
  for (auto& property : properties) {
    if(property.nameHash == hash) {
      return std::get<T>(property.value);
    }
  }
  return T{};
}

template<class T, std::enable_if_t<std::is_same_v<T, Asset>, bool>>
const Asset* Asset::get(uint64_t hash) const {
  for (auto& property : properties) {
    if(property.nameHash == hash) {
      return &std::get<Asset>(property.value);
    }
  }
  return nullptr;
}

}

#endif //INCLUDE_ENJAM_ASSETS_MANAGER_H_
