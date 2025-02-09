#include <enjam/render_view.h>
#include <enjam/renderer_backend.h>

namespace Enjam {

RenderView::RenderView(renderer::RendererBackend& rendererBackend) : rendererBackend(rendererBackend) {
  viewDescriptorSetHandle = rendererBackend.createDescriptorSet(renderer::DescriptorSetData {
      .bindings {
          { .binding = 0, .type = renderer::DescriptorType::UNIFORM_BUFFER }
      }
  });

  viewUniformBufferHandle = rendererBackend.createBufferData(sizeof(PerViewUniforms), renderer::BufferTargetBinding::UNIFORM);
  rendererBackend.updateDescriptorSetBuffer(viewDescriptorSetHandle, 0, viewUniformBufferHandle, sizeof(PerViewUniforms), 0);
}

RenderView::~RenderView() {
  rendererBackend.destroyDescriptorSet(viewDescriptorSetHandle);
  rendererBackend.destroyBufferData(viewUniformBufferHandle);
  rendererBackend.destroyProgram(programHandle);
}

}