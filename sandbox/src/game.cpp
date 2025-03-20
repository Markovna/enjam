#include "game.h"
#include <enjam/application.h>
#include <enjam/simulation.h>
#include <enjam/dependencies.h>
#include <enjam/log.h>
#include <enjam/scene.h>
#include <enjam/render_primitive.h>
#include <enjam/render_view.h>
#include <enjam/renderer.h>
#include <enjam/input.h>
#include <enjam/math.h>
#include <stb_image/stb_image.h>
#include <fstream>
#include <sstream>

class SandboxSimulation : public Enjam::Simulation {
 public:
  SandboxSimulation(Enjam::Renderer &renderer,
                    Enjam::Input &input,
                    Enjam::RendererBackend &rendererBackend,
                    Enjam::Camera &camera,
                    Enjam::Scene &scene)
      : renderer(renderer)
      , input(input)
      , rendererBackend(rendererBackend)
      , camera(camera)
      , scene(scene)
      {}

  void start() override {
    static const float vertexData[] {
        -1, -1,  1,  0,  0,
         1, -1,  1,  1,  0,
        -1,  1,  1,  0,  1,
         1,  1,  1,  1,  1,
        -1, -1, -1,  0,  0,
         1, -1, -1,  1,  0,
        -1,  1, -1,  0,  1,
         1,  1, -1,  1,  1
    };

    static const uint32_t indexData[] {
        2, 6, 7,
        2, 3, 7,
        0, 4, 5,
        0, 1, 5,
        0, 2, 6,
        0, 4, 6,
        1, 3, 7,
        1, 5, 7,
        0, 2, 3,
        0, 1, 3,
        4, 6, 7,
        4, 5, 7
    };

    vertexBuffer = new Enjam::VertexBuffer(
        rendererBackend,
        Enjam::VertexArrayDesc{
            .attributes = {
                Enjam::VertexAttribute{
                    .type = Enjam::VertexAttributeType::FLOAT3,
                    .flags = Enjam::VertexAttribute::FLAG_ENABLED,
                    .offset = 0
                },
                Enjam::VertexAttribute{
                    .type = Enjam::VertexAttributeType::FLOAT2,
                    .flags = Enjam::VertexAttribute::FLAG_ENABLED,
                    .offset = 3 * sizeof(float)
                }
            },
            .stride = 5 * sizeof(float)
        });

    vertexBuffer->setBuffer(rendererBackend, Enjam::BufferDataDesc{(void *) vertexData, sizeof(vertexData)}, 0);

    indexBuffer = new Enjam::IndexBuffer(rendererBackend, 36);
    indexBuffer->setBuffer(rendererBackend, Enjam::BufferDataDesc{(void *) indexData, sizeof(indexData)}, 0);

    std::ifstream vertexShaderFile("shaders/vertex.glsl");
    std::stringstream vertexShaderStrBuffer;
    vertexShaderStrBuffer << vertexShaderFile.rdbuf();

    std::ifstream fragmentShaderFile("shaders/fragment.glsl");
    std::stringstream fragmentShaderStrBuffer;
    fragmentShaderStrBuffer << fragmentShaderFile.rdbuf();

    auto programData = Enjam::ProgramData()
        .setShader(Enjam::ShaderStage::VERTEX, vertexShaderStrBuffer.str().c_str())
        .setShader(Enjam::ShaderStage::FRAGMENT, fragmentShaderStrBuffer.str().c_str())
        .setDescriptorBinding("texture1", Enjam::ProgramData::DescriptorType::TEXTURE, 0)
        .setDescriptorBinding("perView", Enjam::ProgramData::DescriptorType::UNIFORM, 0)
        .setDescriptorBinding("perObject", Enjam::ProgramData::DescriptorType::UNIFORM, 1);

    camera.projectionMatrix = Enjam::math::mat4f::perspective(60, 1.4, 0.1, 10);
    camera.position = Enjam:: math::vec3f { 0, 0, -8 };
    camera.front = Enjam::math::vec3f { 0, 0, 1 };
    camera.up = Enjam::math::vec3f { 0, 1, 0 };

    input.onKeyPress().add(onKeyPress);

    programHandle = rendererBackend.createProgram(programData);

    auto descriptorSetHandle = rendererBackend.createDescriptorSet(Enjam::DescriptorSetData {
        .bindings {
            { .binding = 0, .type = Enjam::DescriptorType::TEXTURE },
        }
    });

    {
      int width, height, nrChannels;
      unsigned char *data = stbi_load("textures/dummy.png", &width, &height, &nrChannels, 0);
      ENJAM_ASSERT(data != nullptr);
      auto th = rendererBackend.createTexture(width, height, 1, Enjam::TextureFormat::RGB8);
      rendererBackend.setTextureData(th, 0, 0, 0, 0, width, height, 0, data);
      stbi_image_free(data);
      rendererBackend.updateDescriptorSetTexture(descriptorSetHandle, 0, th);
    }

    auto triangle1 = Enjam::RenderPrimitive { vertexBuffer, indexBuffer, programHandle };
    triangle1.setDescriptorSetHandle(descriptorSetHandle);
    scene.getPrimitives().push_back(triangle1);

    auto triangle2 = Enjam::RenderPrimitive { vertexBuffer, indexBuffer, programHandle };
    triangle2.setDescriptorSetHandle(descriptorSetHandle);
    triangle2.setTransform(Enjam::math::mat4f::translation(Enjam::math::vec3f {4, 0, 0}));
    scene.getPrimitives().push_back(triangle2);

    ENJAM_INFO("Simulation started!");
  }

  void stop() override {
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

  void tick() override {

  }

 private:
  Enjam::Renderer& renderer;
  Enjam::Input& input;
  Enjam::RendererBackend& rendererBackend;
  Enjam::Camera& camera;
  Enjam::Scene& scene;

  Enjam::VertexBuffer* vertexBuffer = nullptr;
  Enjam::IndexBuffer* indexBuffer = nullptr;
  Enjam::ProgramHandle programHandle = { };

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
        camera.position += Enjam::math::vec3f {0, -0.1, 0};
      }

      if(args.keyCode == KeyCode::Down) {
        camera.position += Enjam::math::vec3f {0, 0.1, 0};
      }
    }};
};

void loadLib(Enjam::Injector& injector) {

  injector.bind<njctr::IFactory<Enjam::Simulation()>>().to<njctr::Factory<SandboxSimulation(Enjam::Renderer&, Enjam::Input&, Enjam::RendererBackend&, Enjam::Camera&, Enjam::Scene&)>>();

  ENJAM_INFO("Game loaded!");
};

void unloadLib(Enjam::Injector&) {

  ENJAM_INFO("Game unloaded!");
}