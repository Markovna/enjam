#include <enjam/asset_manager.h>
#include <enjam/assetfile_parser.h>
#include <enjam/assetfile_serializer.h>
#include <enjam/asset.h>
#include <fstream>
#include <memory>

namespace Enjam {

AssetManager::AssetManager(const Path& rootPath)
  : rootPath(rootPath)
  , assetsByPath()
  { }

AssetManager::Stream AssetManager::loadBufferAsStream(const Path& path, size_t hash) {
  auto fullPath = (rootPath / path).replace_extension() / std::to_string(hash);
  auto stream = std::make_unique<std::ifstream>(fullPath, std::ios::in | std::ios::binary);
  if(!(*stream)) {
    ENJAM_ERROR("Loading buffer failed {}", fullPath.c_str());
    return { };
  }

  return AssetManager::Stream { std::move(stream) };
}

AssetManager::Buffer AssetManager::loadBuffer(const Path& path, size_t hash) {
  auto fullPath = (rootPath / path).replace_extension() / std::to_string(hash);
  auto stream = std::ifstream(fullPath, std::ios::in | std::ios::binary);
  if(!stream) {
    ENJAM_ERROR("Loading buffer failed {}", fullPath.c_str());
    return { };
  }

  AssetManager::Buffer buf;
  stream.seekg(0, std::ios::end);
  auto size = stream.tellg();
  if (size) {
    stream.seekg(0, std::ios::beg);
    buf.resize(size);
    stream.read(&buf.front(), size);
  }

  return buf;
}

void AssetManager::save(const Path& path, Asset& asset) {
  auto fullPath = rootPath / path;

  auto folder = fullPath.parent_path();
  if(!std::filesystem::exists(folder)) {
    std::filesystem::create_directories(folder);
  }

  std::ofstream file(fullPath);
  AssetFileSerializer().setFlags(AssetFileSerializer::pretty | AssetFileSerializer::omitFirstEnclosure).dump(asset, file);
  file.close();
}

size_t AssetManager::saveBuffer(const Path& path, const char* data, size_t size) {
  auto fullPath = (rootPath / path).replace_extension();

  if (!std::filesystem::exists(fullPath) && !std::filesystem::create_directories(fullPath)) {
    ENJAM_ERROR("Saving buffer failed, could not create buffers directory {}", fullPath.string());
    return 0;
  }

  auto hash = std::hash<std::string_view>{}({ data, size });

  auto bufferPath = fullPath / std::to_string(hash);
  std::ofstream file(bufferPath, std::ios::out | std::ios::binary);
  file.write(data, size);
  file.close();

  return hash;
}

std::shared_ptr<Asset> AssetManager::load(const Path& path) {
  std::shared_ptr<Asset> assetPtr;

  auto it = assetsByPath.find(path);
  if(it != assetsByPath.end()) {
    assetPtr = it->second.lock();
  }

  if(!assetPtr) {
    auto fullPath = rootPath / path;
    std::ifstream file(fullPath);

    AssetFileParser parser { file };
    assetPtr = std::make_shared<Asset>(parser());
    assetsByPath[path] = assetPtr;
  }

  return assetPtr;
}

}