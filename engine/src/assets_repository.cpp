#include <enjam/assets_repository.h>
#include <enjam/assetfile_parser.h>
#include <enjam/assetfile_writer.h>
#include <enjam/asset.h>
#include <fstream>
#include <memory>

namespace Enjam {

class AssetFileOutput {
 public:
  explicit AssetFileOutput(const std::filesystem::path& path)
    : path(path), out(path), buffersPath(path) {
    buffersPath.replace_extension();
  }

  template<class TStream, class T, class Enable = void>
  struct has_output_operator : std::false_type { };

  template<class TStream, class T>
  struct has_output_operator<TStream, T,
      std::void_t<decltype(std::declval<TStream>() << std::declval<T>())>>
      : std::true_type {};

  template<class T, std::enable_if_t<has_output_operator<std::ostream&, T>::value, nullptr_t> = nullptr>
  AssetFileOutput& operator<<(T&& arg) {
    out << std::forward<T>(arg);
    return *this;
  }

  AssetFileOutput& operator<<(const Asset::buffer_t& arg) {
    auto buf = arg();
    std::hash<decltype(buf)> hash;

    auto hashStr = fmt::format("{:x}", hash(buf));
    out << '#' << hashStr;

    if(!std::filesystem::exists(buffersPath)) {
      std::filesystem::create_directories(buffersPath);
    }

    auto bufferPath = buffersPath / hashStr;
    std::ofstream file(bufferPath, std::ios::out | std::ios::binary);
    file.write(buf.data(), buf.size());
    file.close();

    return *this;
  }

 private:
  std::filesystem::path path;
  std::filesystem::path buffersPath;
  std::ofstream out;
};

AssetBuffer AssetsFilesystemRep::loadBuffer(const AssetsLoader::Path& path) {
  auto stream = std::ifstream(path, std::ios::in | std::ios::binary);
  if(!stream) {
    ENJAM_ERROR("Loading buffer failed {}", path.c_str());
    return { };
  }

  AssetBuffer buf;
  stream.seekg(0, std::ios::end);
  auto size = stream.tellg();
  if (size) {
    stream.seekg(0, std::ios::beg);
    buf.resize(size);
    stream.read(&buf.front(), size);
  }

  return buf;
}

void AssetsFilesystemRep::save(const Path& path, const Asset& asset) {
  auto fullPath = rootPath / path;

  auto folder = fullPath.parent_path();
  if(!std::filesystem::exists(folder)) {
    std::filesystem::create_directories(folder);
  }

  AssetFileWriter(AssetFileOutput { fullPath }, AssetFileWriterFlags::pretty | AssetFileWriterFlags::omitFirstEnclosure)
    .write(asset);
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
  std::ifstream file(fullPath);

  auto buffersDir = AssetsLoader::Path(fullPath).replace_extension();
  AssetFileParser parser {
    file,
    [&buffersDir](uint64_t hash) -> Asset::buffer_t {
      auto bufferPath = buffersDir / fmt::format("{:x}", hash);
      return [path = bufferPath] { return loadBuffer(path); };
    } };
  return parser();
}

}