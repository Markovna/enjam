#include <enjam/assets_repository.h>
#include <enjam/assetfile_reader.h>
#include <enjam/assetfile_writer.h>
#include <enjam/asset.h>
#include <fstream>
#include <memory>

namespace Enjam {

class AssetFileOutput {
 public:
  explicit AssetFileOutput(const std::filesystem::path& path)
    : path(path), fileOutput(path), buffersDirectory(path) {
    buffersDirectory.replace_extension();
  }

  template<class TStream, class T, class Enable = void>
  struct has_output_operator : std::false_type { };

  template<class TStream, class T>
  struct has_output_operator<TStream, T,
      std::void_t<decltype(std::declval<TStream>() << std::declval<T>())>>
      : std::true_type {};

  template<class T, std::enable_if_t<has_output_operator<std::ostream&, T>::value, nullptr_t> = nullptr>
  AssetFileOutput& operator<<(T&& arg) {
    fileOutput << std::forward<T>(arg);
    return *this;
  }

  AssetFileOutput& operator<<(const AssetBufferLoader& bufferLoader) {
    auto buffer = bufferLoader();
    auto hash = std::hash<ByteArray>{}(buffer);

    fileOutput << fmt::format("{:#x}", hash);

    if(!std::filesystem::exists(buffersDirectory)) {
      std::filesystem::create_directories(buffersDirectory);
    }

    std::ofstream file(buffersDirectory / fmt::format("{:x}", hash), std::ios::out | std::ios::binary);

    std::vector<std::ofstream::char_type> fileBuffer { buffer.begin(), buffer.end() };
    file.write(fileBuffer.data(), fileBuffer.size());
    file.close();

    return *this;
  }

 private:
  std::filesystem::path path;
  std::filesystem::path buffersDirectory;
  std::ofstream fileOutput;
};

class AssetFileInput {
 public:
  explicit AssetFileInput(const std::filesystem::path& path)
      : path(path), fileInput(path), buffersDirectory(path) {
    buffersDirectory.replace_extension();
  }

  std::istream& fileStream() { return fileInput; }

  AssetBufferLoader getBufferLoader(uint64_t hash) {
    return [path = buffersDirectory / fmt::format("{:x}", hash)] () -> ByteArray {
      auto stream = std::ifstream(path, std::ios::in | std::ios::binary);
      if(!stream) {
        ENJAM_ERROR("Loading buffer failed {}", path.c_str());
        return { };
      }

      std::vector<std::ifstream::char_type> buffer;
      stream.seekg(0, std::ios::end);
      auto size = stream.tellg();
      if (size) {
        stream.seekg(0, std::ios::beg);
        buffer.resize(size);

        stream.read(&buffer.front(), size);
      }

      return { buffer.begin(), buffer.end() };
    };
  }

 private:
  std::filesystem::path path;
  std::filesystem::path buffersDirectory;
  std::ifstream fileInput;
};

void AssetsFilesystemRep::save(const Path& path, const Asset& asset) {
  auto fullPath = rootPath / path;

  auto folder = fullPath.parent_path();
  if(!std::filesystem::exists(folder)) {
    std::filesystem::create_directories(folder);
  }

  AssetFileWriter {
    AssetFileOutput { fullPath },
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

Asset AssetsFilesystemRep::load(const AssetsLoader::Path& path) {
  auto fullPath = rootPath / path;

  AssetFileInput fileInput { fullPath };
  Asset asset;
  bool valid = AssetFileReader { fileInput }.parse(asset);
  ENJAM_ASSERT(valid);
  return asset;
}

}