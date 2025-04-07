#include <enjam/asset_manager.h>
#include <enjam/log.h>

int main(int argc, char* argv[]) {
  std::filesystem::path exePath = argv[0];
  std::filesystem::path exeFolder = exePath.parent_path();
  std::filesystem::path filePath = exeFolder / "test.nj_test";

  ENJAM_INFO("File path: {}", filePath.string());

  using namespace Enjam;

  Asset newAsset;
  newAsset["float"] = 3.14;
  newAsset["int"] = -42;
  newAsset["uint"] = 42;

  std::string str("this is str ");
  newAsset["str"] = str;

  Asset& nested = newAsset["object"];
  nested["first"] = " first property of nested object ";

  AssetManager assetManager;
  assetManager.save(filePath, newAsset);
}