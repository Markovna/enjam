#include <enjam/renderer.h>
#include <enjam/renderer_backend.h>
#include <enjam/assert.h>
#include <enjam/math.h>
#include <enjam/render_view.h>
#include <fstream>
#include <sstream>

namespace Enjam {

Renderer::Renderer(RendererBackend& backend)
    : rendererBackend(backend) {
}

struct PerViewUniforms {
  std140::mat44 projection;
  std140::mat44 view;
};

struct DrawData {
  renderer::ProgramHandle programHandle;
  renderer::DescriptorSetHandle viewDescriptorSetHandle;
  renderer::BufferDataHandle viewUniformBufferHandle;
  PerViewUniforms perViewUniformBufferData {};
};

static DrawData drawData;

void Renderer::init() {
  bool initialized = rendererBackend.init();
  ENJAM_ASSERT(initialized && "Failed to initialize renderer backend.");

  std::ifstream vertexShaderFile("shaders/vertex.glsl");
  std::stringstream vertexShaderStrBuffer;
  vertexShaderStrBuffer << vertexShaderFile.rdbuf();

  std::ifstream fragmentShaderFile("shaders/fragment.glsl");
  std::stringstream fragmentShaderStrBuffer;
  fragmentShaderStrBuffer << fragmentShaderFile.rdbuf();

  ProgramData program { };
  program.setShader(ShaderStage::VERTEX, vertexShaderStrBuffer.str().c_str())
        .setShader(ShaderStage::FRAGMENT, fragmentShaderStrBuffer.str().c_str())
        .setDescriptorBinding("perView", 0);

  drawData.programHandle = rendererBackend.createProgram(program);
  drawData.viewDescriptorSetHandle = rendererBackend.createDescriptorSet(renderer::DescriptorSetData {
    .bindings {
      { .binding = 0, .type = renderer::DescriptorType::UNIFORM_BUFFER }
    }
  });

  drawData.viewUniformBufferHandle = rendererBackend.createBufferData(sizeof(PerViewUniforms), renderer::BufferTargetBinding::UNIFORM);
  rendererBackend.updateDescriptorSetBuffer(drawData.viewDescriptorSetHandle, 0, drawData.viewUniformBufferHandle, sizeof(PerViewUniforms), 0);
}

void Renderer::shutdown() {
  rendererBackend.destroyDescriptorSet(drawData.viewDescriptorSetHandle);
  rendererBackend.destroyBufferData(drawData.viewUniformBufferHandle);
  rendererBackend.destroyProgram(drawData.programHandle);
  rendererBackend.shutdown();
}

void Renderer::draw(RenderView& renderView) {
  drawData.perViewUniformBufferData.projection = renderView.camera.projectionMatrix;
  drawData.perViewUniformBufferData.view = math::mat4f::lookAt(renderView.camera.position, renderView.camera.position + renderView.camera.front, renderView.camera.up);

  rendererBackend.beginFrame();
  rendererBackend.updateBufferData(drawData.viewUniformBufferHandle, renderer::BufferDataDesc { &drawData.perViewUniformBufferData, sizeof(drawData.perViewUniformBufferData) }, 0);

  rendererBackend.bindDescriptorSet(drawData.viewDescriptorSetHandle);

  for(auto& primitive : renderView.scene.primitives) {
    rendererBackend.draw(drawData.programHandle, primitive.getVertexBuffer().getHandle(), primitive.getIndexBuffer().getHandle(), 3, 0);
  }

  rendererBackend.endFrame();
}

}