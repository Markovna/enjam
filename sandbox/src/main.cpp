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

void onUnloadLib(Enjam::Library& lib) {
  typedef void (*UnloadFunc)(Enjam::Application& app);
  const std::string funcName = "unloadLib";
  auto funcPtr = reinterpret_cast<UnloadFunc>(lib.getProcAddress(funcName));
  if(!funcPtr) {
    ENJAM_ERROR("Failed to find func {} in {}", funcName, lib.getPath().string());
    return;
  }

  funcPtr(Enjam::Application::get());
}

void onLoadLib(Enjam::Library& lib) {
  typedef void (*LoadFunc)(Enjam::Application& app);
  const std::string funcName = "loadLib";
  auto funcPtr = reinterpret_cast<LoadFunc>(lib.getProcAddress(funcName));
  if(!funcPtr) {
    ENJAM_ERROR("Failed to find func {} in {}", funcName, lib.getPath().string());
    return;
  }

  funcPtr(Enjam::Application::get());
}

int main(int argc, char* argv[]) {
  std::filesystem::path exePath = argv[0];
  std::filesystem::path exeFolder = exePath.parent_path();
  std::string libPath = Enjam::utils::libPath(exeFolder, "game");

  auto libLoader = Enjam::LibraryLoader { createDllCacheDir(exeFolder), onLoadLib, onUnloadLib };
  libLoader.load(libPath);

  auto& app = Enjam::Application::get();

  auto platform = app.getPlatform();
  auto input = app.getInput();
  auto renderer = app.getRenderer();

  input->onKeyPress().add([&app](auto& args){
    using KeyCode = Enjam::KeyCode;
    if(args.keyCode == KeyCode::Escape) {
      app.exit();
    }
  });

  Enjam::RenderView renderView = { };
  renderView.setScene(app.getScene());
  renderView.setCamera(app.getCamera());

  std::unique_ptr<Enjam::Simulation> sim { };

  auto setup = [&app, &sim](){
    sim = app.createSimulation();
    if(sim) {
      sim->start();
    }
  };

  auto cleanup = [&sim](){
    if(sim) {
      sim->stop();
      sim.reset();
    }
  };

  auto tick = [platform, input, renderer, &renderView, &sim](){
    platform->pollInputEvents(*input);
    input->update();

    if(sim) {
      sim->tick();
    }

    renderer->draw(renderView);
  };

  app.run(setup, cleanup, tick);
  ENJAM_INFO("Application stopped running.");
  app.setSimulationFactory(nullptr);
  libLoader.unload(libPath);
  return 0;
}