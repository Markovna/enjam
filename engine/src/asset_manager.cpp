#include <enjam/asset_manager.h>
#include <enjam/assetfile_parser.h>
#include <enjam/asset.h>
#include <fstream>
#include <memory>

namespace Enjam {

AssetManager::AssetManager(const Path& rootPath)
  : rootPath(rootPath)
  , assetsByPath()
  { }

std::shared_ptr<Asset> AssetManager::load(const Path& path) {
  std::shared_ptr<Asset> assetPtr;

  auto it = assetsByPath.find(path);
  if(it != assetsByPath.end()) {
    assetPtr = it->second.lock();
  }

  if(!assetPtr) {
    auto fullPath = rootPath / path;
    std::ifstream file(fullPath);

    assetPtr = std::make_shared<Asset>(AssetFileParser::parse(file));
    assetsByPath[path] = assetPtr;
  }

  return assetPtr;
}

}