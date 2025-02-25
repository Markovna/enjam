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
#include <fstream>

static void hotReload(const std::filesystem::path& path, Enjam::Context& context) {
  typedef void (*GameLoadedFunc)(Enjam::Context& context);
  static auto lib = Enjam::Library { path.string() };

  if(!lib.load()) {
    return;
  }

  const std::string procName = "gameLoaded";
  auto funcPtr = reinterpret_cast<GameLoadedFunc>(lib.getProcAddress(procName));
  if (funcPtr == nullptr) {
    ENJAM_ERROR("Failed to get proc address {}", procName);
    return;
  }

  funcPtr(context);
}

int main(int argc, char* argv[]) {
  std::filesystem::path exePath = argv[0];
  std::filesystem::path libPath = Enjam::utils::libPath(exePath.parent_path().string(), "game");
  ENJAM_INFO("Libs path: {}", libPath.string());

  auto context = Enjam::Context { };

  auto platform = context.getPlatform();
  auto renderer = context.getRenderer();
  auto input = context.getInput();

  hotReload(libPath, context);

  bool isRunning = true;
  input->onKeyPress().add([&](auto args) {
    using KeyCode = Enjam::KeyCode;
    if(args.keyCode == KeyCode::R && args.super) {
      hotReload(libPath, context);
    }
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