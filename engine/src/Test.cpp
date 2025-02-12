#include <enjam/Test.h>
#include <enjam/log.h>
#include <enjam/assert.h>
#include <enjam/input.h>
#include <enjam/library_loader.h>
#include <enjam/platform.h>
#include <enjam/renderer.h>
#include <enjam/render_view.h>
#include <enjam/scene.h>
#include <memory>
#include <fstream>
#include <sstream>

namespace Enjam {

static void bindKeys(Platform& platform, Input& input, bool& isRunning, const std::string& exePath);

struct AppData {
  VertexBuffer* vertexBuffer;
  IndexBuffer* indexBuffer;
  ProgramData programData { };
  Camera camera;
  Scene scene;
};

AppData createAppData(renderer::RendererBackend& rendererBackend) {
  static const float vertexData[] {
      -0.5f, 0.5f, 0.0f,
      0.0f, -0.5f, 0.0f,
      0.5f, 0.5f, 0.0f
  };

  static const uint32_t indexData[] { 0, 1, 2 };

  AppData appData;
  appData.vertexBuffer = new VertexBuffer(
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
  appData.vertexBuffer->setBuffer(
      rendererBackend,
      renderer::BufferDataDesc{(void *) vertexData, sizeof(vertexData)},
      0);

  appData.indexBuffer = new IndexBuffer(rendererBackend, 3);
  appData.indexBuffer->setBuffer(rendererBackend, renderer::BufferDataDesc{(void *) indexData, sizeof(indexData)}, 0);

  std::ifstream vertexShaderFile("shaders/vertex.glsl");
  std::stringstream vertexShaderStrBuffer;
  vertexShaderStrBuffer << vertexShaderFile.rdbuf();

  std::ifstream fragmentShaderFile("shaders/fragment.glsl");
  std::stringstream fragmentShaderStrBuffer;
  fragmentShaderStrBuffer << fragmentShaderFile.rdbuf();

  appData.programData
      .setShader(ShaderStage::VERTEX, vertexShaderStrBuffer.str().c_str())
      .setShader(ShaderStage::FRAGMENT, fragmentShaderStrBuffer.str().c_str())
      .setDescriptorBinding("perView", 0)
      .setDescriptorBinding("perObject", 1);

  appData.camera = {
    .projectionMatrix = math::mat4f::perspective(60, 1.4, 0.1, 10),
    .position = math::vec3f { 0, 0, -1 },
    .front = math::vec3f { 0, 0, 1 },
    .up = math::vec3f { 0, 1, 0 }
  };
  return appData;
}

static AppData appData;



void Test(int argc, char* argv[]) {
  std::string exePath = argv[0];

  Input input { };
  Platform platform { };

  auto rendererBackend = platform.createRendererBackend();
  ENJAM_ASSERT(rendererBackend != nullptr && "Failed to create renderer backend.");

  auto renderer = Renderer { *rendererBackend };
  renderer.init();

  bool isRunning = true;
  appData = createAppData(*rendererBackend);

  appData.scene.getPrimitives().emplace_back(*appData.vertexBuffer, *appData.indexBuffer);

  auto primitive = RenderPrimitive { *appData.vertexBuffer, *appData.indexBuffer };
  primitive.setTransform(math::mat4f::translation(math::vec3f {1, 0, 0}));

  appData.scene.getPrimitives().emplace_back(primitive);

  RenderView renderView;
  renderView.setCamera(&appData.camera);
  renderView.setScene(&appData.scene);
  renderView.setProgram(rendererBackend->createProgram(appData.programData));

  bindKeys(platform, input, isRunning, exePath);

  while(isRunning) {
    platform.pollInputEvents(input);
    input.update();

    renderer.draw(renderView);
  }

  renderer.shutdown();
  platform.shutdown();
}

static void loadLib(LibraryLoader& libLoader, const std::string& libPath, const std::string& name) {
  auto lib = libLoader.load(libPath, name);
  if(!lib) {
    ENJAM_ERROR("Loading dll {} at path {} failed", name, libPath);
    return;
  }

  typedef void (*GameLoadedFunc)();

  auto funcPtr = reinterpret_cast<GameLoadedFunc>(libLoader.getProcAddress(lib, "gameLoaded"));
  if(funcPtr == nullptr) {
    ENJAM_ERROR("Loading dll {} at path {} failed", name, libPath);
    libLoader.free(lib);
    return;
  }

  funcPtr();
}

void bindKeys(Platform& platform, Input& input, bool& isRunning, const std::string& exePath) {
  static std::string libPath { exePath, 0, exePath.find_last_of('/') };

  ENJAM_INFO("{}", libPath);

  input.onKeyPress().add([&](auto args) {
    ENJAM_INFO("Key Press event called: {} {}", (uint16_t) args.keyCode, args.alt);

    if(args.keyCode == KeyCode::Left) {
      appData.camera.position += math::vec3f {0.1, 0, 0};
      ENJAM_INFO("Camera position: {}, {}, {}", appData.camera.position.x, appData.camera.position.y, appData.camera.position.z);
    }

    if(args.keyCode == KeyCode::Right) {
      appData.camera.position += math::vec3f {-0.1, 0, 0};
      ENJAM_INFO("Camera position: {}, {}, {}", appData.camera.position.x, appData.camera.position.y, appData.camera.position.z);
    }

    if(args.keyCode == KeyCode::Up) {
      appData.camera.position += math::vec3f {0, 0, 0.1};
      ENJAM_INFO("Camera position: {}, {}, {}", appData.camera.position.x, appData.camera.position.y, appData.camera.position.z);
    }

    if(args.keyCode == KeyCode::Down) {
      appData.camera.position += math::vec3f {0, 0, -0.1};
      ENJAM_INFO("Camera position: {}, {}, {}", appData.camera.position.x, appData.camera.position.y, appData.camera.position.z);
    }

    if(args.keyCode == KeyCode::R && args.super) {
      loadLib(platform.getLibraryLoader(), libPath, "game");
    }

    if(args.keyCode == KeyCode::Escape) {
      isRunning = false;
    }
  });
}

}
