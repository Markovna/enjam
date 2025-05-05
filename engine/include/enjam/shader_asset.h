#ifndef ENGINE_INCLUDE_ENJAM_SHADER_ASSET_H_
#define ENGINE_INCLUDE_ENJAM_SHADER_ASSET_H_

#include <enjam/asset.h>
#include <enjam/assets_manager.h>
#include <memory>

namespace Enjam {

class ShaderAsset {
 public:
  ShaderAsset(ByteArray&& vert, ByteArray&& frag)
    : vertSource(vert), fragSource(frag)
  { }

  const ByteArray& getSource(ShaderStage stage) {
    switch(stage) {
      case ShaderStage::VERTEX: return vertSource;
      case ShaderStage::FRAGMENT: return fragSource;
    }
    ENJAM_ASSERT(false && "Invalid stage argument in ShaderAsset::getSource");
    return vertSource;
  }

 private:
  ByteArray vertSource;
  ByteArray fragSource;
};

class ShaderAssetFactory {
 private:
  const char* toPropertyName(ShaderLanguage lang) {
    const char* name;
    switch(lang) {
      case ShaderLanguage::GLSL: name = "glsl"; break;
      case ShaderLanguage::SPV: name = "spv"; break;
    }
    return name;
  }
 public:
  AssetRef<ShaderAsset> operator()(const Asset& asset, ShaderLanguage lang) {
    const char* langStr = toPropertyName(lang);
    return std::make_shared<ShaderAsset>(
        asset.at("vert")->at(langStr)->loadBuffer(),
        asset.at("frag")->at(langStr)->loadBuffer()
      );
  }
};

}

#endif //ENGINE_INCLUDE_ENJAM_SHADER_ASSET_H_
