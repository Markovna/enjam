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
class RendererBackend;

class ENJAM_API Renderer final {
 public:
  explicit Renderer(RendererBackend& backend);
  ~Renderer() = default;
  void init();
  void draw(RenderView&);
  void shutdown();

 private:
  RendererBackend& rendererBackend;

  DescriptorSetHandle viewDescriptorSetHandle;
  BufferDataHandle objectsUniformBufferHandle;
  BufferDataHandle viewUniformBufferHandle;
};

}

#endif //INCLUDE_ENJAM_RENDERER_H_
