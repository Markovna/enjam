#include <enjam/context.h>
#include <enjam/log.h>
#include <enjam/assert.h>
#include <enjam/input.h>
#include <enjam/library_loader.h>
#include <enjam/platform.h>
#include <enjam/renderer.h>
#include <enjam/render_view.h>
#include <enjam/scene.h>
#include <memory>
#include <filesystem>

static void hotReload(const std::string& libPath, const std::string& name) {
  static auto libLoader = Enjam::LibraryLoader { };

  typedef void (*GameLoadedFunc)(Enjam::Context& context);

  auto lib = libLoader.load(libPath, name);
  if(!lib) {
    ENJAM_ERROR("Loading dll {} at path {} failed", name, libPath);
    return;
  }

  auto funcPtr = reinterpret_cast<GameLoadedFunc>(libLoader.getProcAddress(lib, "gameLoaded"));
  if(funcPtr == nullptr) {
    ENJAM_ERROR("Loading dll {} at path {} failed", name, libPath);
    libLoader.free(lib);
    return;
  }

  funcPtr(Enjam::Context::get());
}

int main(int argc, char* argv[]) {
  std::filesystem::path exePath = argv[0];
  std::string libPath = exePath.parent_path().string();

  auto& context = Enjam::Context::get();
  context.init();

  auto platform = context.getPlatform();
  auto renderer = context.getRenderer();
  renderer->init();

  auto input = context.getInput();

  auto renderView = Enjam::RenderView { };
  renderView.setCamera(context.getCamera());
  renderView.setScene(context.getScene());

  hotReload(libPath, "game");

  bool isRunning = true;
  input->onKeyPress().add([&](auto args) {
    using KeyCode = Enjam::KeyCode;
    if(args.keyCode == KeyCode::R && args.super) {
      hotReload(libPath, "game");
    }
    if(args.keyCode == KeyCode::Escape) {
      isRunning = false;
    }
  });

  auto app = context.getApp();
  ENJAM_ASSERT(app != nullptr);

  app->setup();

  while(isRunning) {
    platform->pollInputEvents(*input);
    input->update();

    app->tick();

    renderer->draw(renderView);
  }

  app->cleanup();

  renderer->shutdown();
  context.destroy();
}