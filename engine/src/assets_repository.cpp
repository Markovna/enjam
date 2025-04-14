#include <enjam/assets_repository.h>
#include <enjam/assetfile_parser.h>
#include <enjam/assetfile_serializer.h>
#include <enjam/asset.h>
#include <fstream>
#include <memory>

namespace Enjam {

struct BufferFileLoader {
  AssetsLoader::Path path;

  AssetRoot::Buffer operator()(size_t hash) {
    auto fullPath = path / std::to_string(hash);
    auto stream = std::ifstream(fullPath, std::ios::in | std::ios::binary);
    if(!stream) {
      ENJAM_ERROR("Loading buffer failed {}", fullPath.c_str());
      return { };
    }

    AssetRoot::Buffer buf;
    stream.seekg(0, std::ios::end);
    auto size = stream.tellg();
    if (size) {
      stream.seekg(0, std::ios::beg);
      buf.resize(size);
      stream.read(&buf.front(), size);
    }

    return buf;
  }
};

void AssetsFilesystemRep::save(const Path& path, const Asset& asset) {
  auto fullPath = rootPath / path;

  auto folder = fullPath.parent_path();
  if(!std::filesystem::exists(folder)) {
    std::filesystem::create_directories(folder);
  }

  std::ofstream file(fullPath);
  AssetFileSerializer().setFlags(AssetFileSerializer::pretty | AssetFileSerializer::omitFirstEnclosure).dump(asset, file);
  file.close();
}

size_t AssetsFilesystemRep::saveBuffer(const Path& path, const char* data, size_t size) {
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

AssetsRepository::Ref AssetsRepository::load(const Path& path) {
  std::shared_ptr<AssetRoot> assetPtr;

  auto it = assetsByPath.find(path);
  if(it != assetsByPath.end()) {
    assetPtr = it->second.lock();
  }

  if(!assetPtr) {
    assetPtr = std::make_shared<AssetRoot>(loader.load(path));
    assetsByPath[path] = assetPtr;
  }

  return assetPtr;
}

AssetRoot AssetsFilesystemRep::load(const AssetsLoader::Path& path) {
  auto fullPath = rootPath / path;
  std::ifstream file(fullPath);

  AssetFileParser parser { file };
  return AssetRoot {
    .asset = parser(),
    .path = path,
    .bufferLoader = BufferFileLoader { .path = fullPath.replace_extension() }
  };
}

}