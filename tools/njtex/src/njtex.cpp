#include <enjam/assets_repository.h>
#include <stb_image/stb_image.h>
#include <unordered_set>

bool generateAsset(const std::filesystem::path& inputPath, const std::filesystem::path& outputPath) {
  using namespace Enjam;

  const std::unordered_set<std::string> supportedExtensions {
    ".png", ".jpg", ".jpeg", ".tga", ".bmp", ".psd", ".gif", ".hdr", ".pic"
  };

  std::filesystem::path ext = inputPath.extension();
  if(supportedExtensions.find(ext.string()) == supportedExtensions.end()) {
    ENJAM_ERROR("Files with extension {} are not supported", ext.string());
    return false;
  }

  int width, height, channels;
  auto data = stbi_load(inputPath.c_str(), &width, &height, &channels, 0);

  auto size = width * height * channels;
  AssetsFilesystemRep repository;
  auto texDataHash = repository.saveBuffer(outputPath, reinterpret_cast<char*>(data), size);

  Asset asset;
  asset["source"] = inputPath;
  asset["width"] = width;
  asset["height"] = height;
  asset["channels"] = channels;
  asset["data"] = texDataHash;

  repository.save(outputPath, asset);

  stbi_image_free(data);
  return true;
}

int main(int argc, char* argv[]) {
  std::filesystem::path output;
  std::filesystem::path input;

  std::vector<std::string_view> args {argv + 1, argv + argc};

  auto it = args.begin();
  while(it != args.end()) {
    auto& arg = *it;
    if(arg.empty()) {
      it++;
      continue;
    }

    if(arg[0] == '-') {
      if(arg == "-o") {
        it++;
        output = *it;
      } else {
        throw std::runtime_error("Unknown option: " + std::string(*it));
      }
      it++;
      continue;
    }

    input = arg;
    it++;
  }

  if(input.empty()) {
    throw std::runtime_error("Input file path is not provided");
  }

  if(output.empty()) {
    output = input;
    output.replace_extension("nj_tex");
  }

  generateAsset(input, output);
}