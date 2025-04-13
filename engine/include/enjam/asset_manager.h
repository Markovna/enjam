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

namespace Enjam {

class Asset;
class AssetRoot;

class AssetsRepository {
 public:
  using Path = std::filesystem::path;
  using Buffer = std::vector<char>;

  virtual AssetRoot load(const Path& path) = 0;
};

struct AssetRoot {
  using BufferLoader = std::function<AssetsRepository::Buffer(size_t)>;

  Asset asset;
  AssetsRepository::Path path;
  BufferLoader bufferLoader;
};

class AssetsFilesystemRep : public AssetsRepository {
 public:
  explicit AssetsFilesystemRep(Path path = {}) : rootPath(std::move(path)) { }

  AssetRoot load(const Path& path) override;

  void save(const Path& path, const Asset& asset);
  size_t saveBuffer(const Path& path, const char*, size_t);

 private:
  Path rootPath;
};

class AssetsBinaryArchiveRep : public AssetsRepository {
  // TODO
};

class AssetManager {
 public:
  using Path = AssetsRepository::Path;
  using AssetRef = std::shared_ptr<AssetRoot>;

  explicit AssetManager(AssetsRepository& repository)
    : repository(repository)
    , assetsByPath() {

  }

  AssetRef load(const Path& path);

 private:
  std::unordered_map<std::string, std::weak_ptr<AssetRoot>> assetsByPath;
  AssetsRepository& repository;
};

}

#endif //INCLUDE_ENJAM_ASSET_MANAGER_H_
