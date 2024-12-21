#ifndef INCLUDE_ENJAM_RENDERER_BACKEND_H_
#define INCLUDE_ENJAM_RENDERER_BACKEND_H_

#include <enjam/defines.h>

namespace Enjam {

class ENJAM_API RendererBackend {
 public:
  virtual bool init() = 0;
  virtual void shutdown() = 0;
  virtual void beginFrame() = 0;
  virtual void endFrame() = 0;
};

}

#endif //INCLUDE_ENJAM_RENDERER_BACKEND_H_
