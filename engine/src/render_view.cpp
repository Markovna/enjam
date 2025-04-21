#include <enjam/render_view.h>
#include <enjam/scene.h>
#include <enjam/renderer_backend.h>

namespace Enjam {

void RenderView::prepareBuffers() {
  // prepare per view buffer
  perViewUniformBufferData.projection = camera->projectionMatrix;
  perViewUniformBufferData.view = inverse(camera->modelMatrix);

  // prepare per object buffer
  auto& primitives = scene->getPrimitives();
  if(primitives.size() > perObjectUniformBufferData.size()) {
    perObjectUniformBufferData.resize(primitives.size());
  }
  for(auto i = 0; i < primitives.size(); ++i) {
    perObjectUniformBufferData[i].model = primitives[i].getTransform();
  }
}

void RenderView::updateViewUniformBuffer(RendererBackend& rendererBackend, BufferDataHandle handle) {
  rendererBackend.updateBufferData(handle, { &perViewUniformBufferData, sizeof(perViewUniformBufferData) }, 0);
}

void RenderView::updateObjectUniformBuffer(RendererBackend& rendererBackend, BufferDataHandle handle, uint32_t primitiveIndex) {
  rendererBackend.updateBufferData(handle, { &perObjectUniformBufferData[primitiveIndex], sizeof(PerObjectUniforms) }, 0);
}

}