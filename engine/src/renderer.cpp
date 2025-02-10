#include <enjam/renderer.h>
#include <enjam/renderer_backend.h>
#include <enjam/assert.h>
#include <enjam/math.h>
#include <enjam/render_view.h>
#include <enjam/scene.h>

namespace Enjam {

Renderer::Renderer(RendererBackend& backend)
    : rendererBackend(backend) {
}

void Renderer::init() {
  bool initialized = rendererBackend.init();
  ENJAM_ASSERT(initialized && "Failed to initialize renderer backend.");
}

void Renderer::shutdown() {
  rendererBackend.shutdown();
}

void Renderer::draw(RenderView& renderView) {
  rendererBackend.beginFrame();

  renderView.updateViewUniformBuffer();
  renderView.updateObjectsUniformBuffer();

  rendererBackend.bindDescriptorSet(renderView.viewDescriptorSetHandle);

  auto& primitives = renderView.scene->getPrimitives();
  for(auto i = 0; i < primitives.size(); ++i) {
    auto& primitive = primitives[i];

    renderView.updateObjectBuffer(i);
    rendererBackend.draw(renderView.programHandle, primitive.getVertexBuffer().getHandle(), primitive.getIndexBuffer().getHandle(), 3, 0);
  }

  rendererBackend.endFrame();
}

}