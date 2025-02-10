#include <enjam/render_view.h>
#include <enjam/scene.h>
#include <enjam/renderer_backend.h>

namespace Enjam {

RenderView::RenderView(renderer::RendererBackend& rendererBackend) : rendererBackend(rendererBackend) {
  viewDescriptorSetHandle = rendererBackend.createDescriptorSet(renderer::DescriptorSetData {
      .bindings {
          { .binding = 0, .type = renderer::DescriptorType::UNIFORM_BUFFER },
          { .binding = 1, .type = renderer::DescriptorType::UNIFORM_BUFFER }
      }
  });

  viewUniformBufferHandle = rendererBackend.createBufferData(sizeof(PerViewUniforms), renderer::BufferTargetBinding::UNIFORM);
  rendererBackend.updateDescriptorSetBuffer(viewDescriptorSetHandle, 0, viewUniformBufferHandle, sizeof(PerViewUniforms), 0);

  objectsUniformBufferHandle = rendererBackend.createBufferData(sizeof(PerObjectUniforms), renderer::BufferTargetBinding::UNIFORM);
  rendererBackend.updateDescriptorSetBuffer(viewDescriptorSetHandle, 1, objectsUniformBufferHandle, sizeof(PerObjectUniforms), 0);
}

void RenderView::updateViewUniformBuffer() {
  perViewUniformBufferData.projection = camera->projectionMatrix;
  perViewUniformBufferData.view = math::mat4f::lookAt(camera->position, camera->front, camera->up);

  rendererBackend.updateBufferData(viewUniformBufferHandle, { &perViewUniformBufferData, sizeof(perViewUniformBufferData) }, 0);
}

void RenderView::updateObjectsUniformBuffer() {
  auto& primitives = scene->getPrimitives();

  if(primitives.size() > perObjectUniformBufferData.size()) {
    perObjectUniformBufferData.resize(primitives.size());
  }

  for(auto i = 0; i < primitives.size(); ++i) {
    perObjectUniformBufferData[i].model = primitives[i].getTransform();
  }
}

void RenderView::updateObjectBuffer(uint32_t primitiveIndex) {
  rendererBackend.updateBufferData(objectsUniformBufferHandle, { &perObjectUniformBufferData[primitiveIndex], sizeof(PerObjectUniforms) }, 0);
}

RenderView::~RenderView() {
  rendererBackend.destroyDescriptorSet(viewDescriptorSetHandle);
  rendererBackend.destroyBufferData(viewUniformBufferHandle);
  rendererBackend.destroyBufferData(objectsUniformBufferHandle);
}

}