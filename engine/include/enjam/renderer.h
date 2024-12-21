#ifndef INCLUDE_ENJAM_RENDERER_H_
#define INCLUDE_ENJAM_RENDERER_H_

#include <enjam/defines.h>

namespace Enjam {

class RenderView;
class RendererBackend;

class ENJAM_API Renderer {
 public:
  explicit Renderer(RendererBackend&);
  void init();
  void draw(RenderView&);
  void shutdown();

 private:
  RendererBackend& rendererBackend;
};

class RenderView {

};

}

#endif //INCLUDE_ENJAM_RENDERER_H_
