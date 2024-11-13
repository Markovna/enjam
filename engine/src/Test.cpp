#include <enjam/Test.h>
#include <enjam/log.h>
#include <enjam/library_loader.h>

namespace Enjam {

typedef void (*GameLoadedFunc)();

void loadLib(LibraryLoader& libLoader, const std::string& libPath, const std::string& name) {
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

  while(true) {
    char a;
    std::cin >> a;

    loadLib(libLoader, exePath, "game");
  }

}

}
