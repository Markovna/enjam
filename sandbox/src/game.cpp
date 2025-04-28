#include "game.h"
#include <enjam/application.h>
#include <enjam/assets_manager.h>
#include <enjam/assets_repository.h>
#include <enjam/dependencies.h>
#include <enjam/dcc_asset.h>
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
      , textureAssets([&](auto& path) { return assetsRepository.load(path); }, Enjam::TextureAssetFactory { })
      , dccAssets([&](auto& path) { return assetsRepository.load(path); }, Enjam::DCCAssetFactory { })
      {}

  void start() override {

    std::ifstream vertexShaderFile("shaders/simple.vert");
    std::stringstream vertexShaderStrBuffer;
    vertexShaderStrBuffer << vertexShaderFile.rdbuf();

    std::ifstream fragmentShaderFile("shaders/simple.frag");
    std::stringstream fragmentShaderStrBuffer;
    fragmentShaderStrBuffer << fragmentShaderFile.rdbuf();

    auto programData = Enjam::ProgramData()
        .setShader(Enjam::ShaderStage::VERTEX, vertexShaderStrBuffer.str().c_str())
        .setShader(Enjam::ShaderStage::FRAGMENT, fragmentShaderStrBuffer.str().c_str())
        .setDescriptorSet(1, { { "texture1",  Enjam::ProgramData::DescriptorType::SAMPLER } })
        .setDescriptorSet(0, { { "perView",  Enjam::ProgramData::DescriptorType::UNIFORM },
                               { "perObject", Enjam::ProgramData::DescriptorType::UNIFORM }});

    programHandle = rendererBackend.createProgram(programData);

    camera.projectionMatrix = Enjam::math::mat4f::perspective(60, 1.4, 0.1, 100);
    camera.modelMatrix = Enjam::math::mat4f::lookAt(Enjam::math::vec3f { 0, 0, -8 }, Enjam::math::vec3f{ 0, 0, 1 }, Enjam::math::vec3f{ 0, 1, 0 });

    input.onKeyPress().add(onKeyPress);

    auto descriptorSetHandle = rendererBackend.createDescriptorSet(Enjam::DescriptorSetData {
        .bindings {
            { .binding = 0, .type = Enjam::DescriptorType::TEXTURE },
        }
    });

    dummyTex = textureAssets.load("assets/textures/dummy.nj_tex", rendererBackend);
    rendererBackend.updateDescriptorSetTexture(descriptorSetHandle, 0, dummyTex->getHandle());

    cubeAsset = dccAssets.load("assets/models/cube.nj_dcc");
    vertexBuffer.reset(
        new Enjam::VertexBuffer {
          rendererBackend,
          {
            Enjam::VertexAttribute { .type = Enjam::VertexAttributeType::FLOAT3, .offset = 0, .stride = sizeof(Enjam::math::vec3f) },
            Enjam::VertexAttribute { .type = Enjam::VertexAttributeType::FLOAT2, .offset = 0, .stride = sizeof(Enjam::math::vec2f) }
          },
          cubeAsset->getPositions().size()
        });
    vertexBuffer->setBuffer(rendererBackend, 0, Enjam::BufferDataDesc{(void*) cubeAsset->getPositions().data(), cubeAsset->getPositions().size() * sizeof(Enjam::math::vec3f)});
    vertexBuffer->setBuffer(rendererBackend, 1, Enjam::BufferDataDesc{(void*) cubeAsset->getTexCoords0().data(), cubeAsset->getTexCoords0().size() * sizeof(Enjam::math::vec2f)});

    indexBuffer.reset(new Enjam::IndexBuffer(rendererBackend, cubeAsset->getIndices().size()));
    indexBuffer->setBuffer(rendererBackend, Enjam::BufferDataDesc{(void*) cubeAsset->getIndices().data(), cubeAsset->getIndices().size() * sizeof(uint32_t)});

    auto triangle1 = Enjam::RenderPrimitive { vertexBuffer.get(), indexBuffer.get(), programHandle };
    triangle1.setDescriptorSetHandle(descriptorSetHandle);
    scene.getPrimitives().push_back(triangle1);

    auto triangle2 = Enjam::RenderPrimitive { vertexBuffer.get(), indexBuffer.get(), programHandle };
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
    }

    if(indexBuffer) {
      indexBuffer->destroy(rendererBackend);
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

  Enjam::AssetsManager<Enjam::Texture, Enjam::TextureAssetFactory> textureAssets;
  Enjam::AssetsManager<Enjam::DCCAsset, Enjam::DCCAssetFactory> dccAssets;
  Enjam::AssetRef<Enjam::Texture> dummyTex;
  Enjam::AssetRef<Enjam::DCCAsset> cubeAsset;
  std::unique_ptr<Enjam::VertexBuffer> vertexBuffer;
  std::unique_ptr<Enjam::IndexBuffer> indexBuffer;
  Enjam::ProgramHandle programHandle = { };

  Enjam::KeyPressEvent::EventHandler onKeyPress = Enjam::KeyPressEvent::EventHandler {
    [this](const Enjam::KeyPressEventArgs& args) {
      using KeyCode = Enjam::KeyCode;
      if(args.keyCode == KeyCode::A) {
        camera.modelMatrix *= Enjam::math::mat4f::translation(Enjam::math::vec3f {-0.1, 0, 0});
      }

      if(args.keyCode == KeyCode::D) {
        camera.modelMatrix *= Enjam::math::mat4f::translation(Enjam::math::vec3f {0.1, 0, 0});
      }

      if(args.keyCode == KeyCode::W) {
        camera.modelMatrix *= Enjam::math::mat4f::translation(Enjam::math::vec3f {0, 0, -0.1});
      }

      if(args.keyCode == KeyCode::S) {
        camera.modelMatrix *= Enjam::math::mat4f::translation(Enjam::math::vec3f {0, 0, 0.1});
      }

      auto rotationSpeed = 0.04;
      if(args.keyCode == KeyCode::Left) {
        auto rotation = Enjam::math::mat4f::rotation(-rotationSpeed, Enjam::math::vec3f {0, 1, 0});
        camera.modelMatrix *= rotation;
      }

      if(args.keyCode == KeyCode::Right) {
        auto rotation = Enjam::math::mat4f::rotation(rotationSpeed, Enjam::math::vec3f {0, 1, 0});
        camera.modelMatrix *= rotation;
      }

      if(args.keyCode == KeyCode::Up) {
        auto rotation = Enjam::math::mat4f::rotation(-rotationSpeed, Enjam::math::vec3f {-1, 0, 0});
        camera.modelMatrix *= rotation;
      }

      if(args.keyCode == KeyCode::Down) {
        auto rotation = Enjam::math::mat4f::rotation(rotationSpeed, Enjam::math::vec3f {-1, 0, 0});
        camera.modelMatrix *= rotation;
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