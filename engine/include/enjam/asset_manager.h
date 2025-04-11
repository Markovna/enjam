#ifndef INCLUDE_ENJAM_ASSET_MANAGER_H_
#define INCLUDE_ENJAM_ASSET_MANAGER_H_

#include <filesystem>
#include <vector>
#include <unordered_map>
#include <memory>
#include <fstream>
#include <enjam/asset.h>
#include <enjam/utils.h>
#include <enjam/renderer_backend.h>

namespace Enjam {

class Asset;

class AssetManager {
 public:
  using Path = std::filesystem::path;
  using Stream = std::unique_ptr<std::istream>;
  using Buffer = std::vector<char>;

  explicit AssetManager(const Path& rootPath = {});

  std::shared_ptr<Asset> load(const Path& path);
  Stream loadBufferAsStream(const Path&, size_t);

  Buffer loadBuffer(const Path&, size_t);

  void save(const Path& path, Asset& asset);
  size_t saveBuffer(const Path& path, const char*, size_t);

 private:
  Path rootPath;
  std::unordered_map<std::string, std::weak_ptr<Asset>> assetsByPath;
};

}

#endif //INCLUDE_ENJAM_ASSET_MANAGER_H_
