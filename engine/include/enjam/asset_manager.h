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

  explicit AssetManager(const Path& rootPath);

  std::shared_ptr<Asset> load(const Path& path);
  Stream loadBuffer(const Path&, size_t);

 private:
  Path rootPath;
  std::unordered_map<std::string, std::weak_ptr<Asset>> assetsByPath;
};

void load(AssetManager& assetManager, RendererBackend& rendererBackend, const utils::Path& path) {
  auto ptr = assetManager.load(path);
  if(!ptr) {
    return;
  }

  auto& asset = *ptr;
  auto width = asset["width"].as<int>();
  auto height = asset["height"].as<int>();
  auto buffer = assetManager.loadBuffer(path, asset["data"].as<size_t>());

  auto th = rendererBackend.createTexture(width, height, 1, TextureFormat::RGB8);
//  rendererBackend.setTextureData(th, 0, 0, 0, 0, width, height, 0, buffer.);
}

}

#endif //INCLUDE_ENJAM_ASSET_MANAGER_H_
