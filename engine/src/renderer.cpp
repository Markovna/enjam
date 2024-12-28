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
                    .type = VertexAttributeType::FLOAT2,
                    .flags = VertexAttribute::FLAG_ENABLED,
                    .offset = 0
                }
            },
            .stride = 2 * sizeof(float)
        }
    );

    auto ibh = backend.createIndexBuffer(3 * sizeof(uint32_t));
    auto bdh = backend.createBufferData(6 * sizeof(float));

    auto vertexBuf = new float[] {
        -0.5f, 0.5f,
         0.0f, -0.5f,
         0.5f, 0.5f
    };

    auto vertexBufConsumedCallback = [](void* p, uint32_t size) { delete [] static_cast<float*>(p); };
    auto vertexBufDataDesc = BufferDataDesc(vertexBuf, 6 * sizeof(float), vertexBufConsumedCallback);
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

void Renderer::init(RendererBackendType backendType) {
  rendererBackend = platform.createRendererBackend(backendType);
  ENJAM_ASSERT(rendererBackend != nullptr && "Failed to create renderer backend.");

  bool initialized = rendererBackend->init();
  ENJAM_ASSERT(initialized && "Failed to initialize renderer backend.");

  triangle = Triangle::create(*rendererBackend);
}

void Renderer::shutdown() {
  rendererBackend->shutdown();
  delete rendererBackend;
}

void Renderer::draw(RenderView &renderView) {
  rendererBackend->beginFrame();

  rendererBackend->draw(triangle.vbh, triangle.ibh, 3);

  rendererBackend->endFrame();
}

}