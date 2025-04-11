#include <enjam/asset.h>
#include "enjam/type_traits_helpers.h"

namespace Enjam {

Asset& Asset::operator[](const std::string_view& name) {
  auto& obj = ensureType<object_t>();

  std::hash<std::string_view> hash;
  auto nameHash = hash(name);
  auto it = std::find_if(obj.begin(),
                         obj.end(),
                         [&nameHash](auto& prop) { return prop.nameHash == nameHash; });
  if (it != obj.end()) {
    return it->value;
  }

  obj.push_back(Property { .name = std::string { name }, .nameHash = nameHash, .value = { } });
  return obj.back().value;
}

Asset& Asset::operator[](size_t index) {
  auto& array = std::get<array_t>(value);
  return array[index];
}

const Asset* Asset::at(const std::string& name) const {
  if (!is<object_t>()) { return nullptr; }

  auto& obj = std::get<object_t>(value);
  std::hash<std::string> hash;
  auto nameHash = hash(name);
  auto it = std::find_if(obj.begin(),
                         obj.end(),
                         [&nameHash](auto& prop) { return prop.nameHash == nameHash; });
  if (it != obj.end()) {
    return &it->value;
  }

  return nullptr;
}

void Asset::pushBack(const Asset& asset) {
  auto& array = ensureType<array_t>();
  array.push_back(asset);
}

void Asset::pushBack(Asset&& asset) {
  auto& array = ensureType<array_t>();
  array.push_back(std::move(asset));
}

}