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

  viewDescriptorSetHandle = rendererBackend.createDescriptorSet(DescriptorSetData {
      .bindings {
          { .binding = 0, .type = DescriptorType::UNIFORM_BUFFER },
          { .binding = 1, .type = DescriptorType::UNIFORM_BUFFER }
      }
  });

  viewUniformBufferHandle = rendererBackend.createBufferData(sizeof(PerViewUniforms), BufferTargetBinding::UNIFORM);
  rendererBackend.updateDescriptorSetBuffer(viewDescriptorSetHandle, 0, viewUniformBufferHandle, sizeof(PerViewUniforms), 0);

  objectsUniformBufferHandle = rendererBackend.createBufferData(sizeof(PerObjectUniforms), BufferTargetBinding::UNIFORM);
  rendererBackend.updateDescriptorSetBuffer(viewDescriptorSetHandle, 1, objectsUniformBufferHandle, sizeof(PerObjectUniforms), 0);
}

void Renderer::shutdown() {
  rendererBackend.destroyDescriptorSet(viewDescriptorSetHandle);
  rendererBackend.destroyBufferData(viewUniformBufferHandle);
  rendererBackend.destroyBufferData(objectsUniformBufferHandle);

  rendererBackend.shutdown();
}

void Renderer::draw(RenderView& renderView) {
  renderView.prepareBuffers();

  rendererBackend.beginFrame();

  rendererBackend.bindDescriptorSet(viewDescriptorSetHandle, 0);

  renderView.updateViewUniformBuffer(rendererBackend, viewUniformBufferHandle);

  auto& primitives = renderView.scene->getPrimitives();
  for(auto i = 0; i < primitives.size(); ++i) {
    auto& primitive = primitives[i];

    auto dsh = primitive.getDescriptorSetHandle();
    rendererBackend.bindDescriptorSet(dsh, 1);
    renderView.updateObjectUniformBuffer(rendererBackend, objectsUniformBufferHandle, i);
    rendererBackend.draw(primitive.getProgramHandle(), primitive.getVertexBuffer()->getHandle(), primitive.getIndexBuffer()->getHandle());
  }

  rendererBackend.endFrame();
}

}