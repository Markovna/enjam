#include <enjam/renderer.h>
#include <enjam/renderer_backend.h>
#include <enjam/assert.h>
#include <enjam/math.h>
#include <enjam/render_view.h>

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
  renderView.perViewUniformBufferData.projection = renderView.camera->projectionMatrix;
  renderView.perViewUniformBufferData.view = math::mat4f::lookAt(renderView.camera->position, renderView.camera->position + renderView.camera->front, renderView.camera->up);

  rendererBackend.beginFrame();
  rendererBackend.updateBufferData(renderView.viewUniformBufferHandle, renderer::BufferDataDesc { &renderView.perViewUniformBufferData, sizeof(renderView.perViewUniformBufferData) }, 0);

  rendererBackend.bindDescriptorSet(renderView.viewDescriptorSetHandle);

  for(auto& primitive : renderView.primitives) {
    rendererBackend.draw(renderView.programHandle, primitive.getVertexBuffer().getHandle(), primitive.getIndexBuffer().getHandle(), 3, 0);
  }

  rendererBackend.endFrame();
}

}