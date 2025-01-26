#include <enjam/renderer.h>
#include <enjam/renderer_backend.h>
#include <enjam/assert.h>
#include <enjam/platform.h>

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
    auto bdh = backend.createBufferData(9 * sizeof(float));

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

static Triangle triangle;
static ProgramHandle ph;

const char *vertexShaderSource = "#version 410 core\n"
                                 "layout (location = 0) in vec3 aPos;\n"
                                 "layout (std140) uniform perView\n"
                                 "{\n"
                                 "   mat4 projection;\n"
                                 "   mat4 view;\n"
                                 "};\n"
                                 "void main()\n"
                                 "{\n"
                                 "   gl_Position = vec4(aPos, 1.0);\n"
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
        .setShader(ShaderStage::FRAGMENT, fragmentShaderSource);
  ph = rendererBackend->createProgram(program);

  triangle = Triangle::create(*rendererBackend);
}

void Renderer::shutdown() {
  rendererBackend->destroyProgram(ph);
  rendererBackend->shutdown();
  delete rendererBackend;
}

void Renderer::draw(RenderView &renderView) {
  rendererBackend->beginFrame();

  rendererBackend->draw(ph, triangle.vbh, triangle.ibh, 3);

  rendererBackend->endFrame();
}

}