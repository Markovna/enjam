#ifndef INCLUDE_ENJAM_TEXTURE_H_
#define INCLUDE_ENJAM_TEXTURE_H_

#include <enjam/renderer_backend.h>
#include <enjam/assets_manager.h>

namespace Enjam {

class Texture {
 public:
  Texture(RendererBackend& backend, int width, int height)
      : backend(backend), width(width), height(height) {
    handle = backend.createTexture(width, height, 1, Enjam::TextureFormat::RGB8);
  }

  ~Texture() {
    backend.destroyTexture(handle);
  }

  void setBuffer(const char* data) {
    backend.setTextureData(handle, 0, 0, 0, 0, width, height, 0, data);
  }

  const TextureHandle& getHandle() const { return handle; }

 private:
  RendererBackend& backend;
  TextureHandle handle;
  int width;
  int height;
};

template<>
class AssetBuilder<Texture> {
 public:
  explicit AssetBuilder(const AssetRoot& assetRoot) {
    width = assetRoot.asset.at("width")->as<int>();
    height = assetRoot.asset.at("height")->as<int>();
    auto dataHash = assetRoot.asset.at("data")->as<size_t>();
    buffer = assetRoot.bufferLoader(dataHash);
  }

  AssetRef<Texture> operator()(RendererBackend& rendererBackend) {
    auto ptr = std::make_shared<Texture>(rendererBackend, width, height);
    ptr->setBuffer(buffer.data());
    return ptr;
  }

 private:
  int width;
  int height;
  AssetRoot::Buffer buffer;
};

}

#endif //INCLUDE_ENJAM_TEXTURE_H_
