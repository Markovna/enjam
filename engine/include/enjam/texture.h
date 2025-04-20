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

  void setBuffer(const void* data) {
    backend.setTextureData(handle, 0, 0, 0, 0, width, height, 0, data);
  }

  [[nodiscard]] const TextureHandle& getHandle() const { return handle; }

 private:
  RendererBackend& backend;
  TextureHandle handle;
  int width;
  int height;
};

struct TextureAssetFactory {
  AssetRef<Texture> operator()(const Asset& asset) {
    auto width = asset.at("width")->as<int>();
    auto height = asset.at("height")->as<int>();
    auto buffer = asset.at("data")->loadBuffer();

    auto ptr = std::make_shared<Texture>(rendererBackend, width, height);
    ptr->setBuffer(buffer.data());
    return ptr;
  }

  RendererBackend& rendererBackend;
};

}

#endif //INCLUDE_ENJAM_TEXTURE_H_
