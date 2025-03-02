#include "game.h"
#include <enjam/application.h>
#include <enjam/log.h>
#include <enjam/scene.h>
#include <enjam/render_primitive.h>
#include <enjam/render_view.h>
#include <enjam/renderer.h>
#include <enjam/input.h>
#include <enjam/math.h>
#include <fstream>
#include <sstream>

class SandboxSimulation : public Enjam::Simulation {
 public:
  SandboxSimulation(Enjam::Renderer &renderer,
                    Enjam::Input &input,
                    Enjam::renderer::RendererBackend &rendererBackend,
                    Enjam::Camera &camera,
                    Enjam::Scene &scene)
      : renderer(renderer)
      , input(input)
      , rendererBackend(rendererBackend)
      , camera(camera)
      , scene(scene)
      {}

  ~SandboxSimulation() {
    ENJAM_INFO("~SandboxSimulation");
  }

  void start() {
    static const float vertexData[] {
        -0.5f, 0.5f, 0.0f,
        0.0f, -0.5f, 0.0f,
        0.5f, 0.5f, 0.0f
    };

    static const uint32_t indexData[] { 0, 1, 2 };

    vertexBuffer = new Enjam::VertexBuffer(
        rendererBackend,
        Enjam::renderer::VertexArrayDesc{
            .attributes = {
                Enjam::renderer::VertexAttribute{
                    .type = Enjam::renderer::VertexAttributeType::FLOAT3,
                    .flags = Enjam::renderer::VertexAttribute::FLAG_ENABLED,
                    .offset = 0
                }
            },
            .stride = 3 * sizeof(float)
        });

    vertexBuffer->setBuffer(
        rendererBackend,
        Enjam::renderer::BufferDataDesc{(void *) vertexData, sizeof(vertexData)},
        0);

    indexBuffer = new Enjam::IndexBuffer(rendererBackend, 3);
    indexBuffer->setBuffer(rendererBackend, Enjam::renderer::BufferDataDesc{(void *) indexData, sizeof(indexData)}, 0);

    std::ifstream vertexShaderFile("shaders/vertex.glsl");
    std::stringstream vertexShaderStrBuffer;
    vertexShaderStrBuffer << vertexShaderFile.rdbuf();

    std::ifstream fragmentShaderFile("shaders/fragment.glsl");
    std::stringstream fragmentShaderStrBuffer;
    fragmentShaderStrBuffer << fragmentShaderFile.rdbuf();

    auto programData = Enjam::ProgramData()
        .setShader(Enjam::ShaderStage::VERTEX, vertexShaderStrBuffer.str().c_str())
        .setShader(Enjam::ShaderStage::FRAGMENT, fragmentShaderStrBuffer.str().c_str())
        .setDescriptorBinding("perView", 0)
        .setDescriptorBinding("perObject", 1);

    camera.projectionMatrix = Enjam::math::mat4f::perspective(60, 1.4, 0.1, 10);
    camera.position = Enjam:: math::vec3f { 0, 0, -3 };
    camera.front = Enjam::math::vec3f { 0, 0, 1 };
    camera.up = Enjam::math::vec3f { 0, 1, 0 };

    input.onKeyPress().add(onKeyPress);

    programHandle = rendererBackend.createProgram(programData);

    scene.getPrimitives().emplace_back( vertexBuffer, indexBuffer, programHandle );

    auto triangle = Enjam::RenderPrimitive { vertexBuffer, indexBuffer, programHandle };
    triangle.setTransform(Enjam::math::mat4f::translation(Enjam::math::vec3f {1, 0, 0}));

    scene.getPrimitives().push_back(triangle);

    ENJAM_INFO("Simulation started!");
  }

  void stop() {
    auto& primitives = scene.getPrimitives();
    primitives.clear();

    if(vertexBuffer) {
      vertexBuffer->destroy(rendererBackend);
      delete vertexBuffer;
      vertexBuffer = nullptr;
    }

    if(indexBuffer) {
      indexBuffer->destroy(rendererBackend);
      delete indexBuffer;
      indexBuffer = nullptr;
    }

    if(programHandle) {
      rendererBackend.destroyProgram(programHandle);
      programHandle = { };
    }

    input.onKeyPress().remove(onKeyPress);

    ENJAM_INFO("Simulation stopped!");
  }

  void tick() {

  }

 private:
  Enjam::Renderer& renderer;
  Enjam::Input& input;
  Enjam::renderer::RendererBackend& rendererBackend;
  Enjam::Camera& camera;
  Enjam::Scene& scene;

  Enjam::VertexBuffer* vertexBuffer = nullptr;
  Enjam::IndexBuffer* indexBuffer = nullptr;
  Enjam::renderer::ProgramHandle programHandle = { };

  Enjam::KeyPressEvent::EventHandler onKeyPress = Enjam::KeyPressEvent::EventHandler {
    [this](const Enjam::KeyPressEventArgs& args) {
      using KeyCode = Enjam::KeyCode;
      if(args.keyCode == KeyCode::Left) {
        camera.position += Enjam::math::vec3f {0.1, 0, 0};
      }

      if(args.keyCode == KeyCode::Right) {
        camera.position += Enjam::math::vec3f {-0.1, 0, 0};
      }

      if(args.keyCode == KeyCode::Up) {
        camera.position += Enjam::math::vec3f {0, 0, 0.1};
      }

      if(args.keyCode == KeyCode::Down) {
        camera.position += Enjam::math::vec3f {0, 0, -0.1};
      }
    }};
};

void loadLib(Enjam::Application& app) {
  auto& renderer = *app.getRenderer();
  auto& input = *app.getInput();
  auto& rendererBackend = *app.getRendererBackend();
  auto& camera = *app.getCamera();
  auto& scene = *app.getScene();

  app.setSimulationFactory([&renderer, &input, &rendererBackend, &camera, &scene]() {
    return std::make_unique<SandboxSimulation>(renderer, input, rendererBackend, camera, scene);
  });

};

void unloadLib(Enjam::Application& app) {

  ENJAM_INFO("Game unloaded!");
}