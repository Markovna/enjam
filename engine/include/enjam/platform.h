#ifndef INCLUDE_ENJAM_PLATFORM_H_
#define INCLUDE_ENJAM_PLATFORM_H_

#include <enjam/defines.h>
#include <enjam/renderer_backend_type.h>

//#if defined(ENJAM_PLATFORM_WINDOWS)
//#include <enjam/platform_glfw.h>
//
//#elif defined(ENJAM_PLATFORM_DARWIN)
//#include <enjam/platform_glfw.h>
//
//#endif

namespace Enjam {

class Input;

namespace renderer {
class RendererBackend;
}

class Platform {
 public:
  virtual ~Platform() = default;

  virtual renderer::RendererBackend* createRendererBackend(RendererBackendType = RendererBackendType::DEFAULT) = 0;
  virtual void pollInputEvents(Input&) = 0;

 protected:
  Platform() = default;
};

}

#endif //INCLUDE_ENJAM_PLATFORM_H_