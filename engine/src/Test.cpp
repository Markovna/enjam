#include <enjam/Test.h>
#include <enjam/log.h>
#include <enjam/assert.h>
#include <enjam/input.h>
#include <enjam/library_loader.h>
#include <enjam/platform.h>
#include <enjam/renderer.h>
#include <enjam/render_view.h>
#include <memory>

namespace Enjam {

static void bindKeys(Input& input, RenderView&, bool& isRunning, const std::string& exePath);
RenderPrimitive createTriangle(renderer::RendererBackend& rendererBackend);

void Test(int argc, char* argv[]) {
  std::string exePath = argv[0];

  auto input = Input { };
  auto platform = Platform { input };
  platform.init();

  auto rendererBackend = platform.createRendererBackend();
  ENJAM_ASSERT(rendererBackend != nullptr && "Failed to create renderer backend.");

  auto renderer = Renderer { *rendererBackend };
  renderer.init();

  bool isRunning = true;

  RenderView renderView {
    .camera = {
        .projectionMatrix = math::mat4f::perspective(60, 1.4, 0.1, 10),
        .position = math::vec3f { 0, 0, -1 },
        .front = math::vec3f { 0, 0, 1 },
        .up = math::vec3f { 0, 1, 0 }
    }
  };

  auto triangle = createTriangle(*rendererBackend);
  renderView.scene.primitives.push_back(triangle);

  bindKeys(input, renderView, isRunning, exePath);

  while(isRunning) {
    platform.pollInputEvents();
    input.update();

    renderer.draw(renderView);
  }

  renderer.shutdown();
  platform.shutdown();
}

static void loadLib(LibraryLoader& libLoader, const std::string& libPath, const std::string& name) {
  libLoader.free();

  bool dllLoaded = libLoader.load(libPath, name);
  if(!dllLoaded) {
    ENJAM_ERROR("Loading dll {} at path {} failed", name, libPath);
    return;
  }

  typedef void (*GameLoadedFunc)();

  auto* funcPtr = reinterpret_cast<GameLoadedFunc>(libLoader.getProcAddress("gameLoaded"));
  if(funcPtr == nullptr) {
    ENJAM_ERROR("Loading dll {} at path {} failed", name, libPath);
    libLoader.free();
    return;
  }

  funcPtr();
}

RenderPrimitive createTriangle(renderer::RendererBackend& rendererBackend) {
  static const float vertexData[] {
      -0.5f, 0.5f, 0.0f,
      0.0f, -0.5f, 0.0f,
      0.5f, 0.5f, 0.0f
  };

  static const uint32_t indexData[] { 0, 1, 2 };

  static VertexBuffer vertexBuffer = VertexBuffer(
      rendererBackend,
      renderer::VertexArrayDesc{
          .attributes = {
              renderer::VertexAttribute{
                  .type = renderer::VertexAttributeType::FLOAT3,
                  .flags = renderer::VertexAttribute::FLAG_ENABLED,
                  .offset = 0
              }
          },
          .stride = 3 * sizeof(float)
      });
  vertexBuffer.setBuffer(
      rendererBackend,
      renderer::BufferDataDesc { (void*) vertexData, sizeof(vertexData)},
      0);

  static IndexBuffer indexBuffer = IndexBuffer(rendererBackend, 3);
  indexBuffer.setBuffer(rendererBackend, renderer::BufferDataDesc { (void*) indexData, sizeof(indexData) }, 0);

  return { vertexBuffer, indexBuffer };
}

void bindKeys(Input& input, RenderView& renderView, bool& isRunning, const std::string& exePath) {
  static std::string libPath { exePath, 0, exePath.find_last_of('/') };

  ENJAM_INFO("{}", libPath);

  static LibraryLoader libLoader {};

  input.onKeyPress().add([&](auto args) {
    ENJAM_INFO("Key Press event called: {} {}", (uint16_t) args.keyCode, args.alt);

    if(args.keyCode == KeyCode::Left) {
      renderView.camera.position += math::vec3f {0.1, 0, 0};
    }

    if(args.keyCode == KeyCode::Right) {
      renderView.camera.position += math::vec3f {-0.1, 0, 0};
    }

    if(args.keyCode == KeyCode::Up) {
      renderView.camera.position += math::vec3f {0, 0, 0.1};
    }

    if(args.keyCode == KeyCode::Down) {
      renderView.camera.position += math::vec3f {0, 0, -0.1};
    }

    if(args.keyCode == KeyCode::R && args.super) {
      loadLib(libLoader, libPath, "game");
    }

    if(args.keyCode == KeyCode::Escape) {
      isRunning = false;
    }
  });
}

}
