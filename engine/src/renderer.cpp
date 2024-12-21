#include <enjam/renderer.h>
#include <enjam/renderer_backend.h>
#include <enjam/assert.h>

namespace Enjam {

Renderer::Renderer(RendererBackend &rendererBackend)
    : rendererBackend(rendererBackend) {
}

void Renderer::init() {
  bool initialized = rendererBackend.init();
  ENJAM_ASSERT(initialized && "Failed to initialize renderer backend.");
}

void Renderer::shutdown() {
  rendererBackend.shutdown();
}

void Renderer::draw(RenderView &renderView) {
  rendererBackend.beginFrame();

  rendererBackend.endFrame();
}

}