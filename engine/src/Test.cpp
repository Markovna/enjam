#include <enjam/Test.h>
#include <enjam/log.h>
#include <enjam/assert.h>
#include <enjam/input.h>
#include <enjam/library_loader.h>
#include <enjam/library_loader.h>
#include <enjam/platform.h>
#include <enjam/engine.h>

namespace Enjam {

typedef void (*GameLoadedFunc)();

static void loadLib(LibraryLoader& libLoader, const std::string& libPath, const std::string& name) {
  libLoader.free();

  bool dllLoaded = libLoader.load(libPath, name);
  if(!dllLoaded) {
    ENJAM_ERROR("Loading dll {} at path {} failed", name, libPath);
    return;
  }

  auto* funcPtr = reinterpret_cast<GameLoadedFunc>(libLoader.getProcAddress("gameLoaded"));
  if(funcPtr == nullptr) {
    ENJAM_ERROR("Loading dll {} at path {} failed", name, libPath);
    libLoader.free();
    return;
  }

  funcPtr();
}

void Test(int argc, char* argv[]) {
  std::string exePath = argv[0];
  exePath.erase(exePath.find_last_of('/'));

  ENJAM_INFO("{}", exePath);
  LibraryLoader libLoader {};

  Platform platform {};
  Engine engine {};

  platform.init(engine);

  bool isRunning = true;

  engine.getInput().onKeyPress().add([&](auto args) {
    ENJAM_INFO("Key Press event called: {} {}", (uint16_t) args.keyCode, args.alt);

    if(args.keyCode == KeyCode::R && args.alt) {
      loadLib(libLoader, exePath, "game");
    }

    if(args.keyCode == KeyCode::Escape) {
      isRunning = false;
    }
  });

  while(isRunning) {
    platform.pollInputEvents();
    engine.update();
  }

  platform.shutdown();

}

}
