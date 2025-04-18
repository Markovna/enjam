#ifndef ENGINE_INCLUDE_ENJAM_ASSET_BUFFER_H_
#define ENGINE_INCLUDE_ENJAM_ASSET_BUFFER_H_

#include <vector>
#include <string_view>

namespace Enjam {

using AssetBuffer = std::vector<char>;

}

namespace std {

template<>
struct hash<Enjam::AssetBuffer> {
  size_t operator()(const Enjam::AssetBuffer& buf) const {
      return hash<string_view>{}({ buf.data(), buf.size() });
  }
};

}

#endif //ENGINE_INCLUDE_ENJAM_ASSET_BUFFER_H_
