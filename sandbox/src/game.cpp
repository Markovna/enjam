#include "game.h"
#include <enjam/log.h>
#include <enjam/context.h>
#include <enjam/scene.h>
#include <enjam/render_primitive.h>
#include <enjam/render_view.h>
#include <enjam/renderer.h>
#include <enjam/input.h>
#include <enjam/math.h>
#include <fstream>
#include <sstream>

class GameApp : public Enjam::App {
 public:
  GameApp(Enjam::Camera& camera, Enjam::Scene& scene, Enjam::renderer::RendererBackend& rendererBackend, Enjam::Input& input)
    : camera(camera), scene(scene), input(input), rendererBackend(rendererBackend)
  {

  }

  void setup() override {
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
    camera.position =Enjam:: math::vec3f { 0, 0, -1 };
    camera.front = Enjam::math::vec3f { 0, 0, 1 };
    camera.up = Enjam::math::vec3f { 0, 1, 0 };

    input.onKeyPress().add([&](auto args) {
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
    });

    programHandle = rendererBackend.createProgram(programData);
    scene.getPrimitives().emplace_back(*vertexBuffer, *indexBuffer, programHandle);

    auto primitive = Enjam::RenderPrimitive { *vertexBuffer, *indexBuffer, programHandle };
    primitive.setTransform(Enjam::math::mat4f::translation(Enjam::math::vec3f {1, 0, 0}));

    scene.getPrimitives().emplace_back(primitive);

    ENJAM_INFO("Game setup finished.");
  }

  void tick() override {

  }

  void cleanup() override {

  }

 private:
  Enjam::Camera& camera;
  Enjam::Scene& scene;
  Enjam::Input& input;
  Enjam::renderer::RendererBackend& rendererBackend;

  Enjam::VertexBuffer* vertexBuffer;
  Enjam::IndexBuffer* indexBuffer;
  Enjam::renderer::ProgramHandle programHandle;
};

void gameLoaded(Enjam::Context& context) {
  ENJAM_INFO("Game loaded!");

  auto app = new GameApp { *context.getCamera(), *context.getScene(), *context.getRendererBackend(), *context.getInput() };
  context.setApp(app);
}