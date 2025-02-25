#include <enjam/context.h>
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

static void load(const std::string& path, Enjam::Context& context) {
  typedef void (*GameLoadedFunc)(Enjam::Context& context);

  auto lib = Enjam::utils::loadLib(path);
  if(!lib) {
    ENJAM_ERROR("Loading dll at path {} failed", path);
    return;
  }

  auto funcPtr = reinterpret_cast<GameLoadedFunc>(Enjam::utils::getProcAddress(lib, "gameLoaded"));
  if(funcPtr == nullptr) {
    ENJAM_ERROR("Loading dll at path {} failed", path);
    Enjam::utils::freeLib(lib);
    return;
  }

  funcPtr(context);
}

int main(int argc, char* argv[]) {
  std::filesystem::path exePath = argv[0];
  std::string libPath = Enjam::utils::libPath(exePath.parent_path().string(), "game");

  auto context = Enjam::Context { };

  auto platform = context.getPlatform();
  auto renderer = context.getRenderer();
  auto input = context.getInput();

  load(libPath, context);

  bool isRunning = true;
  input->onKeyPress().add([&](auto args) {
    using KeyCode = Enjam::KeyCode;
    if(args.keyCode == KeyCode::Escape) {
      isRunning = false;
    }
  });

  auto app = context.getApp();
  ENJAM_ASSERT(app != nullptr);

  renderer->init();
  app->setup();

  auto renderView = Enjam::RenderView { };
  renderView.setCamera(context.getCamera());
  renderView.setScene(context.getScene());

  while(isRunning) {
    platform->pollInputEvents(*input);
    input->update();

    app->tick();

    renderer->draw(renderView);
  }

  app->cleanup();
  renderer->shutdown();
}