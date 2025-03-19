#ifndef INCLUDE_ENJAM_PLATFORM_H_
#define INCLUDE_ENJAM_PLATFORM_H_

#include <enjam/defines.h>
#include <enjam/renderer_backend_type.h>
#include <memory>


namespace Enjam {

class Input;
class RendererBackend;

class Platform {
 public:
  virtual ~Platform() = default;

  virtual std::unique_ptr<RendererBackend> createRendererBackend(RendererBackendType = RendererBackendType::DEFAULT) = 0;
  virtual void pollInputEvents(Input&) = 0;

 protected:
  Platform() = default;
};

}

#endif //INCLUDE_ENJAM_PLATFORM_H_