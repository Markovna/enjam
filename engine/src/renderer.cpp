#include <enjam/renderer.h>
#include <enjam/renderer_backend.h>
#include <enjam/assert.h>
#include <enjam/platform.h>
#include <enjam/math.h>

namespace Enjam {

Renderer::Renderer(Platform& platform)
    : platform(platform) {
}

struct Triangle {
  VertexBufferHandle vbh;
  IndexBufferHandle ibh;
  BufferDataHandle bdh;

  static Triangle create(RendererBackend& backend) {
    auto vbh = backend.createVertexBuffer(
        VertexArrayDesc{
            .attributes = {
                VertexAttribute{
                    .type = VertexAttributeType::FLOAT3,
                    .flags = VertexAttribute::FLAG_ENABLED,
                    .offset = 0
                }
            },
            .stride = 3 * sizeof(float)
        }
    );

    auto ibh = backend.createIndexBuffer(3 * sizeof(uint32_t));
    auto bdh = backend.createBufferData(9 * sizeof(float), BufferTargetBinding::VERTEX);

    auto vertexBuf = new float[] {
        -0.5f, 0.5f, 0.0f,
         0.0f, -0.5f, 0.0f,
         0.5f, 0.5f, 0.0f
    };

    auto vertexBufConsumedCallback = [](void* p, uint32_t size) { delete [] static_cast<float*>(p); };
    auto vertexBufDataDesc = BufferDataDesc(vertexBuf, 9 * sizeof(float), vertexBufConsumedCallback);
    backend.updateBufferData(bdh, std::move(vertexBufDataDesc), 0);
    backend.assignVertexBufferData(vbh, bdh);

    auto indexBuf = new uint32_t [] {0, 1, 2};

    auto indexBufConsumedCallback = [](void* p, uint32_t size) { delete [] static_cast<uint32_t*>(p); };
    auto indexBufDataDesc = BufferDataDesc(indexBuf, 3 * sizeof(uint32_t), indexBufConsumedCallback);
    backend.updateIndexBuffer(ibh, std::move(indexBufDataDesc), 0);

    return Triangle {
        .vbh = vbh,
        .ibh = ibh,
        .bdh = bdh
    };
  }
};

struct PerViewUniforms {
  std140::mat44 projection;
  std140::mat44 view;
};

struct DrawData {
  Triangle triangle;
  ProgramHandle programHandle;
  DescriptorSetHandle viewDescriptorSetHandle;
  BufferDataHandle viewUniformBufferHandle;
  PerViewUniforms perViewUniformBufferData;
};

static DrawData drawData;

const char *vertexShaderSource = "#version 410 core\n"
                                 "layout (location = 0) in vec3 aPos;\n"
                                 "layout (std140) uniform perView\n"
                                 "{\n"
                                 "   mat4 projection;\n"
                                 "   mat4 view;\n"
                                 "};\n"
                                 "void main()\n"
                                 "{\n"
                                 "   gl_Position = projection * view * vec4(aPos, 1.0);\n"
                                 "}\0";
const char *fragmentShaderSource = "#version 410 core\n"
                                   "out vec4 FragColor;\n"
                                   "void main()\n"
                                   "{\n"
                                   "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
                                   "}\n\0";

void Renderer::init(RendererBackendType backendType) {
  rendererBackend = platform.createRendererBackend(backendType);
  ENJAM_ASSERT(rendererBackend != nullptr && "Failed to create renderer backend.");

  bool initialized = rendererBackend->init();
  ENJAM_ASSERT(initialized && "Failed to initialize renderer backend.");

  ProgramData program { };
  program.setShader(ShaderStage::VERTEX, vertexShaderSource)
        .setShader(ShaderStage::FRAGMENT, fragmentShaderSource)
        .setDescriptorBinding("perView", 0);

  drawData.programHandle = rendererBackend->createProgram(program);
  drawData.viewDescriptorSetHandle = rendererBackend->createDescriptorSet(DescriptorSetData {
    .bindings { { .binding = 0, .type = DescriptorType::UNIFORM_BUFFER } }
  });

  drawData.viewUniformBufferHandle = rendererBackend->createBufferData(sizeof(PerViewUniforms), BufferTargetBinding::UNIFORM);
  rendererBackend->updateDescriptorSetBuffer(drawData.viewDescriptorSetHandle, 0, drawData.viewUniformBufferHandle, sizeof(PerViewUniforms), 0);

  drawData.triangle = Triangle::create(*rendererBackend);
}

void Renderer::shutdown() {
  rendererBackend->destroyProgram(drawData.programHandle);
  rendererBackend->shutdown();
  delete rendererBackend;
}

void Renderer::draw(RenderView& renderView) {
  drawData.perViewUniformBufferData.projection = renderView.projectionMatrix;
  drawData.perViewUniformBufferData.view = math::mat4f::lookAt(renderView.position, renderView.position + renderView.front, renderView.up);

  rendererBackend->beginFrame();

  rendererBackend->bindDescriptorSet(drawData.viewDescriptorSetHandle);

  rendererBackend->updateBufferData(drawData.viewUniformBufferHandle, BufferDataDesc { &drawData.perViewUniformBufferData, sizeof(drawData.perViewUniformBufferData) }, 0);
  rendererBackend->draw(drawData.programHandle, drawData.triangle.vbh, drawData.triangle.ibh, 3);

  rendererBackend->endFrame();
}

}