#ifndef INCLUDE_ENJAM_ASSET_MANAGER_H_
#define INCLUDE_ENJAM_ASSET_MANAGER_H_

#include <filesystem>
#include <utility>
#include <vector>
#include <unordered_map>
#include <memory>
#include <fstream>
#include <enjam/asset.h>
#include <enjam/utils.h>
#include <enjam/renderer_backend.h>

// TODO:
// 1. Support reference assets by UUID
// 2. Implement Assets Binary Archive loading

namespace Enjam {

class Asset;
class AssetRoot;

class AssetsLoader {
 public:
  using Path = std::filesystem::path;

  virtual AssetRoot load(const Path& path) = 0;
};

struct AssetRoot {
  using Buffer = std::vector<char>;
  using BufferLoader = std::function<Buffer(size_t)>;

  Asset asset;
  AssetsLoader::Path path;
  BufferLoader bufferLoader;
};

class AssetsFilesystemRep : public AssetsLoader {
 public:
  explicit AssetsFilesystemRep(Path path = {}) : rootPath(std::move(path)) { }

  AssetRoot load(const Path& path) override;

  void save(const Path& path, const Asset& asset);
  size_t saveBuffer(const Path& path, const char*, size_t);

 private:
  Path rootPath;
};

class AssetsBinaryArchiveRep : public AssetsLoader {
  // TODO
};

class AssetsRepository {
 public:
  using Path = AssetsLoader::Path;
  using Ref = std::shared_ptr<AssetRoot>;

  explicit AssetsRepository(AssetsLoader& loader)
    : loader(loader)
    , assetsByPath() {

  }

  Ref load(const Path& path);

 private:
  std::unordered_map<std::string, std::weak_ptr<AssetRoot>> assetsByPath;
  AssetsLoader& loader;
};

}

#endif //INCLUDE_ENJAM_ASSET_MANAGER_H_
