#ifndef INCLUDE_ENJAM_RENDERER_H_
#define INCLUDE_ENJAM_RENDERER_H_

#include <enjam/defines.h>
#include <enjam/assert.h>
#include <enjam/platform.h>
#include <enjam/renderer_backend_type.h>
#include <enjam/math.h>
#include <enjam/render_primitive.h>
#include <vector>

namespace Enjam {

class RenderView;

namespace renderer {

class RendererBackend;

}

class ENJAM_API Renderer final {
 public:
  using RendererBackend = renderer::RendererBackend;

  explicit Renderer(RendererBackend& backend);
  ~Renderer() = default;
  void init();
  void draw(RenderView&);
  void shutdown();

 private:
  RendererBackend& rendererBackend;
};

}

#endif //INCLUDE_ENJAM_RENDERER_H_
