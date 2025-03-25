#ifndef INCLUDE_ENJAM_ASSET_MANAGER_H_
#define INCLUDE_ENJAM_ASSET_MANAGER_H_

#include <filesystem>
#include <vector>
#include <memory>
#include <fstream>
#include <enjam/asset.h>

namespace Enjam {

class Asset;

class AssetBuffer {
 public:
  AssetBuffer(char* data, size_t size) : buffer() {
    buffer.reserve(size);
    for (auto i = 0; i < size; ++i) {
      buffer.push_back(data[i]);
    }
  }

 private:
  std::vector<char> buffer;
};

template<class T>
struct AssetSerialization;

class AssetManager {
 public:
  using Path = std::filesystem::path;

  explicit AssetManager(const Path& rootPath);

  Asset loadAsset(const Path& path);


 private:
  std::filesystem::path rootPath;
};

}

#endif //INCLUDE_ENJAM_ASSET_MANAGER_H_
