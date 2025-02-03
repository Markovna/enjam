#include <enjam/Test.h>
#include <enjam/log.h>
#include <enjam/assert.h>
#include <enjam/input.h>
#include <enjam/library_loader.h>
#include <enjam/platform.h>
#include <enjam/renderer.h>
#include <enjam/renderer_backend.h>
#include <enjam/renderer_backend_type.h>
#include <memory>

namespace Enjam {

static void bindKeys(Input& input, RenderView&, bool& isRunning, const std::string& exePath);

void Test(int argc, char* argv[]) {
  std::string exePath = argv[0];

  auto input = Input { };
  auto platform = Platform { input };
  auto renderer = Renderer { platform };

  platform.init();
  renderer.init();

  bool isRunning = true;

  RenderView renderView { };
  renderView.projectionMatrix = math::mat4f::perspective(60, 1.4, 0.1, 10);
  renderView.position = math::vec3f { 0, 0, -1 };
  renderView.front = math::vec3f { 0, 0, 1 };
  renderView.up = math::vec3f { 0, 1, 0 };

  bindKeys(input, renderView, isRunning, exePath);

  while(isRunning) {
    platform.pollInputEvents();
    input.update();

    renderer.draw(renderView);
  }

  renderer.shutdown();
  platform.shutdown();
}

static void loadLib(LibraryLoader& libLoader, const std::string& libPath, const std::string& name) {
  libLoader.free();

  bool dllLoaded = libLoader.load(libPath, name);
  if(!dllLoaded) {
    ENJAM_ERROR("Loading dll {} at path {} failed", name, libPath);
    return;
  }

  typedef void (*GameLoadedFunc)();

  auto* funcPtr = reinterpret_cast<GameLoadedFunc>(libLoader.getProcAddress("gameLoaded"));
  if(funcPtr == nullptr) {
    ENJAM_ERROR("Loading dll {} at path {} failed", name, libPath);
    libLoader.free();
    return;
  }

  funcPtr();
}

void bindKeys(Input& input, RenderView& renderView, bool& isRunning, const std::string& exePath) {
  static std::string libPath { exePath, 0, exePath.find_last_of('/') };

  ENJAM_INFO("{}", libPath);

  static LibraryLoader libLoader {};

  input.onKeyPress().add([&](auto args) {
    ENJAM_INFO("Key Press event called: {} {}", (uint16_t) args.keyCode, args.alt);


    if(args.keyCode == KeyCode::Left) {
      renderView.position += math::vec3f {0.1, 0, 0};
    }

    if(args.keyCode == KeyCode::Right) {
      renderView.position += math::vec3f {-0.1, 0, 0};
    }


    if(args.keyCode == KeyCode::Up) {
      renderView.position += math::vec3f {0, 0, 0.1};
    }

    if(args.keyCode == KeyCode::Down) {
      renderView.position += math::vec3f {0, 0, -0.1};
    }

    if(args.keyCode == KeyCode::R && args.super) {
      loadLib(libLoader, libPath, "game");
    }

    if(args.keyCode == KeyCode::Escape) {
      isRunning = false;
    }
  });
}

}
