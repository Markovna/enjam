#include "game.h"
#include <enjam/application.h>
#include <enjam/assets_manager.h>
#include <enjam/assets_repository.h>
#include <enjam/dependencies.h>
#include <enjam/input.h>
#include <enjam/log.h>
#include <enjam/math.h>
#include <enjam/scene.h>
#include <enjam/simulation.h>
#include <enjam/texture.h>
#include <enjam/renderer.h>
#include <enjam/render_primitive.h>
#include <enjam/render_view.h>
#include <sstream>

// TODO:
// 1. Replace all the manual assets loading work with the Resource Managers
// 2. Move Input handling to tick, check Input state instead of event handlers
// 3. Smooth camera movement, utilizing delta time
// 4. Camera rotation on <- ->
// 5. Models loading instead of primitives

class SandboxSimulation : public Enjam::Simulation {
 public:
  SandboxSimulation(Enjam::Renderer &renderer,
                    Enjam::AssetsRepository& assetsRepository,
                    Enjam::Input &input,
                    Enjam::RendererBackend& rendererBackend,
                    Enjam::Camera& camera,
                    Enjam::Scene& scene)
      : renderer(renderer)
      , assetsRepository(assetsRepository)
      , input(input)
      , rendererBackend(rendererBackend)
      , camera(camera)
      , scene(scene)
      , textureAssets([&](auto& path) { return assetsRepository.load(path); })
      {}

  void start() override {
    static const float vertexData[] {
        -1, -1,  1,    0,  0,
         1, -1,  1,    1,  0,
        -1,  1,  1,    0,  1,
         1,  1,  1,    1,  1,
        -1, -1, -1,    0,  0,
         1, -1, -1,    1,  0,
        -1,  1, -1,    0,  1,
         1,  1, -1,    1,  1
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
        {
            Enjam::VertexAttribute { .type = Enjam::VertexAttributeType::FLOAT3, .offset = 0,                 .stride = 5 * sizeof(float) },
            Enjam::VertexAttribute { .type = Enjam::VertexAttributeType::FLOAT2, .offset = 3 * sizeof(float), .stride = 5 * sizeof(float) }
        },
        8);

    vertexBuffer->setBuffer(rendererBackend, 0, Enjam::BufferDataDesc{(void*) vertexData, sizeof(vertexData)});
    vertexBuffer->setBuffer(rendererBackend, 1, Enjam::BufferDataDesc{(void*) vertexData, sizeof(vertexData)});

    indexBuffer = new Enjam::IndexBuffer(rendererBackend, 36);
    indexBuffer->setBuffer(rendererBackend, Enjam::BufferDataDesc{(void*) indexData, sizeof(indexData)});

    std::ifstream vertexShaderFile("shaders/vertex.glsl");
    std::stringstream vertexShaderStrBuffer;
    vertexShaderStrBuffer << vertexShaderFile.rdbuf();

    std::ifstream fragmentShaderFile("shaders/fragment.glsl");
    std::stringstream fragmentShaderStrBuffer;
    fragmentShaderStrBuffer << fragmentShaderFile.rdbuf();

    auto programData = Enjam::ProgramData()
        .setShader(Enjam::ShaderStage::VERTEX, vertexShaderStrBuffer.str().c_str())
        .setShader(Enjam::ShaderStage::FRAGMENT, fragmentShaderStrBuffer.str().c_str())
        .setDescriptorSet(1, { { "texture1",  Enjam::ProgramData::DescriptorType::SAMPLER } })
        .setDescriptorSet(0, { { "perView",  Enjam::ProgramData::DescriptorType::UNIFORM },
                               { "perObject", Enjam::ProgramData::DescriptorType::UNIFORM }});

    programHandle = rendererBackend.createProgram(programData);

    camera.projectionMatrix = Enjam::math::mat4f::perspective(60, 1.4, 0.1, 10);
    camera.position = Enjam:: math::vec3f { 0, 0, -8 };
    camera.front = Enjam::math::vec3f { 0, 0, 1 };
    camera.up = Enjam::math::vec3f { 0, 1, 0 };

    input.onKeyPress().add(onKeyPress);

    auto descriptorSetHandle = rendererBackend.createDescriptorSet(Enjam::DescriptorSetData {
        .bindings {
            { .binding = 0, .type = Enjam::DescriptorType::TEXTURE },
        }
    });

    dummyTex = textureAssets.load("assets/textures/dummy.nj_tex", Enjam::TextureAssetFactory { .rendererBackend = rendererBackend } );
    rendererBackend.updateDescriptorSetTexture(descriptorSetHandle, 0, dummyTex->getHandle());

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
  Enjam::AssetsRepository& assetsRepository;
  Enjam::Renderer& renderer;
  Enjam::Input& input;
  Enjam::RendererBackend& rendererBackend;
  Enjam::Camera& camera;
  Enjam::Scene& scene;
  Enjam::AssetsManager<Enjam::Texture> textureAssets;

  Enjam::AssetRef<Enjam::Texture> dummyTex;

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

  injector.bind<njctr::IFactory<Enjam::Simulation()>>().to<njctr::Factory<SandboxSimulation(Enjam::Renderer&, Enjam::AssetsRepository&, Enjam::Input&, Enjam::RendererBackend&, Enjam::Camera&, Enjam::Scene&)>>();

  ENJAM_INFO("Game loaded!");
};

void unloadLib(Enjam::Injector&) {

  ENJAM_INFO("Game unloaded!");
}