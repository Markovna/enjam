#ifndef INCLUDE_RENDERER_BACKEND_OPENGL_H_
#define INCLUDE_RENDERER_BACKEND_OPENGL_H_

#include "renderer_backend.h"
#include <functional>
#include <enjam/platform.h>

namespace Enjam {

class RendererBackendOpengl : public RendererBackend {
 public:
  explicit RendererBackendOpengl(Platform& platform);

  bool init() override;
  void shutdown() override;
  void beginFrame() override;
  void endFrame() override;

 private:
  Platform& platform;
  uint32_t vertexArrays;
};

}

#endif //INCLUDE_RENDERER_BACKEND_OPENGL_H_
