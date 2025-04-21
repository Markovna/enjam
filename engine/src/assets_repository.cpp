#include <enjam/assets_repository.h>
#include <enjam/assetfile_reader.h>
#include <enjam/assetfile_writer.h>
#include <enjam/asset.h>
#include <fstream>
#include <memory>

namespace Enjam {

struct AssetFileOutput {
 public:
  using BufferStream = std::unique_ptr<std::ostream>;
  using BufferStreamProvider = std::function<BufferStream(uint64_t)>;

  std::ostream& out;
  BufferStreamProvider buffers;
};

struct AssetFileInput {
 public:
  using BufferStream = std::unique_ptr<std::istream>;
  using BufferStreamProvider = std::function<BufferStream(uint64_t)>;

  std::istream& input;
  BufferStreamProvider buffers;
};

void AssetsFilesystemRep::save(const Path& path, const Asset& asset) {
  auto fullPath = rootPath / path;

  auto directory = fullPath.parent_path();
  if(!std::filesystem::exists(directory)) {
    std::filesystem::create_directories(directory);
  }

  std::ofstream file(fullPath);
  AssetFileOutput output {
    .out = file,
    .buffers = [this, path](uint64_t hash) -> std::unique_ptr<std::ostream> {
      return getBufferOutput(path, hash);
    }
  };
  AssetFileWriter {
    output,
    AssetFileWriterFlags::pretty | AssetFileWriterFlags::omitFirstEnclosure
  }.write(asset);
}

AssetsRepository::Ref AssetsRepository::load(const Path& path) {
  std::shared_ptr<Asset> assetPtr;

  auto it = assetsByPath.find(path);
  if(it != assetsByPath.end()) {
    assetPtr = it->second.lock();
  }

  if(!assetPtr) {
    assetPtr = std::make_shared<Asset>(loader.load(path));
    assetsByPath[path] = assetPtr;
  }

  return assetPtr;
}


std::unique_ptr<std::istream> AssetsFilesystemRep::getBufferInput(const Path& assetPath, uint64_t hash) {
  auto path = (rootPath / assetPath).replace_extension() / fmt::format("{:x}", hash);
  auto stream = std::make_unique<std::ifstream>(path, std::ios::in | std::ios::binary);
  if (stream->fail()) {
    ENJAM_ERROR("Loading buffer failed {}", path.c_str());
    return { };
  }
  return stream;
}

std::unique_ptr<std::ostream> AssetsFilesystemRep::getBufferOutput(const Path& assetPath, uint64_t hash) {
  auto buffersPath = (rootPath / assetPath).replace_extension();
  if(!std::filesystem::exists(buffersPath)) {
    std::filesystem::create_directories(buffersPath);
  }

  auto path = buffersPath / fmt::format("{:x}", hash);
  auto file = std::make_unique<std::ofstream>(path, std::ios::out | std::ios::binary);
  if(file->fail()) {
    ENJAM_ERROR("Saving buffer failed {}", path.c_str());
    return { };
  }
  return file;
}

Asset AssetsFilesystemRep::load(const AssetsLoader::Path& path) {
  auto fullPath = rootPath / path;

  std::ifstream file(fullPath);
  AssetFileInput fileInput {
      .input = file,
      .buffers = [this, path](uint64_t hash) -> std::unique_ptr<std::istream> {
        return getBufferInput(path, hash);
      }
  };
  Asset asset;
  bool valid = AssetFileReader { fileInput }.parse(asset);
  ENJAM_ASSERT(valid);
  return asset;
}

}