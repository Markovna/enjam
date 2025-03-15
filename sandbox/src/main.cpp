#include <enjam/application.h>
#include <enjam/log.h>
#include <enjam/assert.h>
#include <enjam/input.h>
#include <enjam/library_loader.h>
#include <enjam/platform.h>
#include <enjam/renderer.h>
#include <enjam/render_view.h>
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

Enjam::LibraryLoader CreateLibLoader(const Enjam::utils::Path& dllCacheDir, Enjam::Dependencies& dependencies) {
  auto onLoadLib = [&dependencies](Enjam::Library& lib) {
    typedef void (*LoadFunc)(Enjam::Dependencies&);
    const std::string funcName = "loadLib";
    auto funcPtr = reinterpret_cast<LoadFunc>(lib.getProcAddress(funcName));
    if(!funcPtr) {
      ENJAM_ERROR("Failed to find func {} in {}", funcName, lib.getPath().string());
      return;
    }

    funcPtr(dependencies);
  };

  auto onUnloadLib = [&dependencies](Enjam::Library& lib) {
    typedef void (*UnloadFunc)(Enjam::Dependencies&);
    const std::string funcName = "unloadLib";
    auto funcPtr = reinterpret_cast<UnloadFunc>(lib.getProcAddress(funcName));
    if (!funcPtr) {
      ENJAM_ERROR("Failed to find func {} in {}", funcName, lib.getPath().string());
      return;
    }

    funcPtr(dependencies);
  };

  return Enjam::LibraryLoader { dllCacheDir, onLoadLib, onUnloadLib };
}

int main(int argc, char* argv[]) {
  std::filesystem::path exePath = argv[0];
  std::filesystem::path exeFolder = exePath.parent_path();
  std::string libPath = Enjam::utils::libPath(exeFolder, "game");

  auto dependencies = Enjam::Dependencies { };
  auto libLoader = CreateLibLoader(createDllCacheDir(exeFolder), dependencies);
  libLoader.load(libPath);

  auto app = Enjam::Application { };
  auto platform = Enjam::PlatformGlfw {};
  auto rendererBackend = platform.createRendererBackend();
  auto renderer = Enjam::Renderer(*rendererBackend);
  auto input = Enjam::Input {};
  auto scene = Enjam::Scene {};
  auto camera = Enjam::Camera {};

  auto setupDependencies = [&]() {
    dependencies.bind<Enjam::Renderer>().to(renderer);
    dependencies.bind<Enjam::Input>().to(input);
    dependencies.bind<Enjam::renderer::RendererBackend>().to(*rendererBackend);
    dependencies.bind<Enjam::Scene>().to(scene);
    dependencies.bind<Enjam::Camera>().to(camera);
  };

  setupDependencies();

  bool hotReload = false;

  input.onKeyPress().add([&hotReload, &app](auto& args){
    using KeyCode = Enjam::KeyCode;
    if(args.keyCode == KeyCode::Escape) {
      app.exit();
    }

    if(args.keyCode == KeyCode::R && args.super) {
      ENJAM_INFO("Requested hot reload");
      hotReload = true;
    }
  });

  Enjam::RenderView renderView = { };
  renderView.setScene(&scene);
  renderView.setCamera(&camera);

  std::unique_ptr<Enjam::Simulation> sim { };
  auto simulationFactory = dependencies.resolve<enjector::IFactory<Enjam::Simulation>&>();

  auto setup = [&renderer, &simulationFactory, &sim](){
    renderer.init();

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

    renderer.shutdown();
  };

  auto tick = [&hotReload, &libLoader, &libPath, &dependencies, &platform, &input, &renderer, &renderView, &sim, setupDependencies, &simulationFactory](){
    platform.pollInputEvents(input);
    input.update();

    if(hotReload) {
      sim->stop();
      sim.reset();

      dependencies.clear();
      setupDependencies();

      libLoader.load(libPath);

      simulationFactory = dependencies.resolve<enjector::IFactory<Enjam::Simulation>&>();

      sim = simulationFactory();
      sim->start();

      hotReload = false;
    }

    if(sim) {
      sim->tick();
    }

    renderer.draw(renderView);
  };

  app.run(setup, cleanup, tick);

  libLoader.unload(libPath);
  return 0;
}