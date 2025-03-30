#include <enjam/asset.h>

namespace Enjam {

Asset& Asset::operator[](const std::string& name) {
  if(!std::holds_alternative<object_t>(value)) {
    value = object_t { };
  }

  auto& obj = std::get<object_t>(value);

  std::hash<std::string> hash;
  auto nameHash = hash(name);
  auto it = std::find_if(obj.begin(),
                         obj.end(),
                         [&nameHash](auto& prop) { return prop.nameHash == nameHash; });
  if (it != obj.end()) {
    return it->value;
  }

  obj.push_back(Property { .nameHash = nameHash, .value = { } });
  return obj.back().value;
}

Asset& Asset::operator[](size_t index) {
  auto& array = std::get<array_t>(value);
  return array[index];
}

const Asset* Asset::at(const std::string& name) const {
  if(!std::holds_alternative<object_t>(value)) {
    return nullptr;
  }

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

}