#ifndef INCLUDE_ENJAM_ASSET_MANAGER_H_
#define INCLUDE_ENJAM_ASSET_MANAGER_H_

#include <filesystem>
#include <fstream>
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>
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

  virtual Asset load(const Path& path) = 0;
};

class AssetsFilesystemRep : public AssetsLoader {
 public:
  explicit AssetsFilesystemRep(Path path = {}) : rootPath(std::move(path)) { }

  Asset load(const Path& path) override;
  void save(const Path& path, const Asset& asset);

 private:
  static AssetBuffer loadBuffer(const AssetsLoader::Path&);

 private:
  Path rootPath;
};

class AssetsBinaryArchiveRep : public AssetsLoader {
  // TODO
};

class AssetsRepository {
 public:
  using Path = AssetsLoader::Path;
  using Ref = std::shared_ptr<Asset>;

  explicit AssetsRepository(AssetsLoader& loader)
    : loader(loader), assetsByPath() { }

  Ref load(const Path& path);

 private:
  std::unordered_map<std::string, std::weak_ptr<Asset>> assetsByPath;
  AssetsLoader& loader;
};

}

#endif //INCLUDE_ENJAM_ASSET_MANAGER_H_
