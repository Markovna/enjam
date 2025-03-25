#include <enjam/asset_manager.h>
#include <enjam/assetfile_parser.h>
#include <enjam/asset.h>
#include <fstream>

namespace Enjam {

AssetManager::AssetManager(const Path& rootPath)
  : rootPath(rootPath) { }

Asset AssetManager::loadAsset(const Path& path) {
  auto fullPath = rootPath / path;
  std::ifstream file(fullPath);
  return AssetFileParser::parse(file);
}

}