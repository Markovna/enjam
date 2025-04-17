#include <enjam/assets_repository.h>
#include <enjam/application.h>
#include <enjam/log.h>
#include <enjam/assert.h>
#include <enjam/input.h>
#include <enjam/library_loader.h>
#include <enjam/platform.h>
#include <enjam/renderer.h>
#include <enjam/render_view.h>
#include <enjam/simulation.h>
#include <enjam/scene.h>
#include <enjam/utils.h>
#include <enjam/dependencies.h>
#include <enjam/platform_glfw.h>
#include <memory>
#include <filesystem>

Enjam::utils::Path createDllCacheDir(const Enjam::utils::Path& currentPath) {
  auto path = currentPath / "dll-cache";
  if(!std::filesystem::exists(path)) {
    bool success = std::filesystem::create_directory(path);
    if(!success) {
      ENJAM_ERROR("Failed to create directory {}", path.string());
      return { };
    }
  }
  return path;
}

Enjam::LibraryLoader CreateLibLoader(const Enjam::utils::Path& dllCacheDir, Enjam::Injector& injector) {
  auto onLoadLib = [&injector](Enjam::Library& lib) {
    typedef void (*LoadFunc)(Enjam::Injector&);
    const std::string funcName = "loadLib";
    auto funcPtr = reinterpret_cast<LoadFunc>(lib.getProcAddress(funcName));
    if(!funcPtr) {
      ENJAM_ERROR("Failed to find func {} in {}", funcName, lib.getPath().string());
      return;
    }

    funcPtr(injector);
  };

  auto onUnloadLib = [&injector](Enjam::Library& lib) {
    typedef void (*UnloadFunc)(Enjam::Injector&);
    const std::string funcName = "unloadLib";
    auto funcPtr = reinterpret_cast<UnloadFunc>(lib.getProcAddress(funcName));
    if (!funcPtr) {
      ENJAM_ERROR("Failed to find func {} in {}", funcName, lib.getPath().string());
      return;
    }

    funcPtr(injector);
  };

  return Enjam::LibraryLoader { dllCacheDir, onLoadLib, onUnloadLib };
}

int main(int argc, char* argv[]) {
  std::filesystem::path exePath = argv[0];
  std::filesystem::path exeFolder = exePath.parent_path();
  std::string libPath = Enjam::utils::libPath(exeFolder, "game");

  auto injector = Enjam::Injector { };
  auto libLoader = CreateLibLoader(createDllCacheDir(exeFolder), injector);
  libLoader.load(libPath);

  auto app = std::make_shared<Enjam::Application>();
  auto platform = std::make_shared<Enjam::PlatformGlfw>();
  std::shared_ptr<Enjam::RendererBackend> rendererBackend = platform->createRendererBackend();
  auto renderer = std::make_shared<Enjam::Renderer>(*rendererBackend);
  auto input = std::make_shared<Enjam::Input>();
  auto scene = std::make_shared<Enjam::Scene>();
  auto camera = std::make_shared<Enjam::Camera>();
  auto assetsLoader = std::make_shared<Enjam::AssetsFilesystemRep>(exeFolder);
  auto assetsRepository = std::make_shared<Enjam::AssetsRepository>(*assetsLoader);

  auto setupDependencies = [&]() {
    injector.bind<Enjam::AssetsRepository>().to(assetsRepository);
    injector.bind<Enjam::Renderer>().to(renderer);
    injector.bind<Enjam::Input>().to(input);
    injector.bind<Enjam::RendererBackend>().to(rendererBackend);
    injector.bind<Enjam::Scene>().to(scene);
    injector.bind<Enjam::Camera>().to(camera);
  };

  setupDependencies();

  bool hotReload = false;

  input->onKeyPress().add([&hotReload, &app](auto& args){
    using KeyCode = Enjam::KeyCode;
    if(args.keyCode == KeyCode::Escape) {
      app->exit();
    }

    if(args.keyCode == KeyCode::R && args.super) {
      hotReload = true;
    }
  });

  Enjam::RenderView renderView = { };
  renderView.setScene(scene.get());
  renderView.setCamera(camera.get());

  std::unique_ptr<Enjam::Simulation> sim { };
  auto simulationFactory = injector.resolve<njctr::IFactory<Enjam::Simulation()>>();

  auto setup = [&renderer, &simulationFactory, &sim](){
    renderer->init();

    sim = simulationFactory();
    if(sim) {
      sim->start();
    }
  };

  auto cleanup = [&renderer, &sim](){
    if(sim) {
      sim->stop();
      sim.reset();
    }

    renderer->shutdown();
  };

  auto tick = [&hotReload, &libLoader, &libPath, &injector, &platform, &input, &renderer, &renderView, &sim, setupDependencies, &simulationFactory](){
    platform->pollInputEvents(*input);
    input->update();

    if(hotReload) {
      sim->stop();
      sim.reset();

      injector.reset();
      setupDependencies();

      libLoader.load(libPath);

      simulationFactory = injector.resolve<njctr::IFactory<Enjam::Simulation()>>();

      sim = simulationFactory();
      sim->start();

      hotReload = false;
    }

    if(sim) {
      sim->tick();
    }

    renderer->draw(renderView);
  };

  app->run(setup, cleanup, tick);

  injector.reset();
  libLoader.unload(libPath);
  return 0;
}