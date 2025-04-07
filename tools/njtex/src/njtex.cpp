#include <enjam/asset_manager.h>
#include <stb_image/stb_image.h>
#include <unordered_set>

void generateAsset(const std::filesystem::path& inputPath, const std::filesystem::path& outputPath) {
  using namespace Enjam;

  const std::unordered_set<std::string> supportedExtensions {
    ".png", ".jpg", ".jpeg", ".tga", ".bmp", ".psd", ".gif", ".hdr", ".pic"
  };

  std::filesystem::path ext = inputPath.extension();
  if(supportedExtensions.find(ext.string()) == supportedExtensions.end()) {
    ENJAM_ERROR("Files with extension {} are not supported", ext.string());
    return;
  }

  int width, height, channels;
  auto data = stbi_load(inputPath.c_str(), &width, &height, &channels, 0);

  auto size = width * height * channels;
  AssetManager assetManager;
  auto texDataHash = assetManager.saveBuffer(outputPath, reinterpret_cast<char*>(data), size);

  Asset asset;
  asset["source"] = inputPath;
  asset["width"] = width;
  asset["height"] = height;
  asset["channels"] = channels;
  asset["data"] = texDataHash;

  assetManager.save(outputPath, asset);

  stbi_image_free(data);
}

int main(int argc, char* argv[]) {
  using namespace Enjam;

  std::filesystem::path output;
  std::filesystem::path input;

  char option = 0;
  for(auto i = 0; i < argc; i++) {
    ENJAM_INFO("{}", argv[i]);
    if(argv[i][0] == '-') {
      option = argv[i][1];
    }

  }


//  generateAsset(
//      "/Users/kamila/CLionProjects/enjam/sandbox/textures/dummy.png",
//      "/Users/kamila/CLionProjects/enjam/sandbox/assets/textures/dummy.nj_tex"
//      );
}