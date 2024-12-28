#ifndef INCLUDE_ENJAM_RENDERER_H_
#define INCLUDE_ENJAM_RENDERER_H_

#include <enjam/defines.h>
#include <enjam/platform.h>
#include <enjam/renderer_backend_type.h>

namespace Enjam {

class RenderView;
class RendererBackend;

class ENJAM_API Renderer final {
 public:
  explicit Renderer(Platform& platform);
  ~Renderer() = default;
  void init(RendererBackendType = DEFAULT);
  void draw(RenderView&);
  void shutdown();

 private:
  Platform& platform;
  RendererBackend* rendererBackend;
};

class RenderView {

};

}

#endif //INCLUDE_ENJAM_RENDERER_H_
