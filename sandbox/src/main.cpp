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

typedef void (*GameLoadedFunc)(Enjam::Context& context);

static Enjam::utils::Path createDllCacheDir() {
  auto path = std::filesystem::current_path() / "dll-cache";
  if(!std::filesystem::exists(path)) {
    bool success = std::filesystem::create_directory(path);
    if(!success) {
      ENJAM_ERROR("Failed to create directory {}", path.string());
      return { };
    }
  }
  return path;
}

static void load(const std::string& path, Enjam::Context& context) {
  static auto libLoader = Enjam::LibraryLoader { createDllCacheDir() };
  libLoader.load(path, context);
}

int main(int argc, char* argv[]) {
  std::filesystem::path exePath = argv[0];
  std::string libPath = Enjam::utils::libPath(exePath.parent_path(), "game");

  auto context = Enjam::Context { };

  auto platform = context.getPlatform();
  auto renderer = context.getRenderer();
  auto input = context.getInput();

  load(libPath, context);

  bool isRunning = true;
  input->onKeyPress().add([&](auto args) {
    using KeyCode = Enjam::KeyCode;
    if(args.keyCode == KeyCode::R && args.super) {
      load(libPath, context);
    }

    if(args.keyCode == KeyCode::Escape) {
      isRunning = false;
    }
  });

  renderer->init();
  context.createApp();

  auto renderView = Enjam::RenderView { };
  renderView.setCamera(context.getCamera());
  renderView.setScene(context.getScene());

  while(isRunning) {
    platform->pollInputEvents(*input);
    input->update();

    context.tickApp();

    renderer->draw(renderView);
  }

  context.destroyApp();
  renderer->shutdown();
}