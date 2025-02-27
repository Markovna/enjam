#include <enjam/utils.h>
#include <enjam/defines.h>
#ifdef ENJAM_PLATFORM_WINDOWS
#include <windows.h>
#else
#include <dlfcn.h>
#endif

#if ENJAM_COMPILER_MSVC || ENJAM_COMPILER_CLANG
#define LIB_PREFIX ""
#else
#define LIB_PREFIX "lib"
#endif

#if defined(ENJAM_PLATFORM_WINDOWS)
#define LIB_EXTENSION_NAME "dll"
#elif defined(ENJAM_PLATFORM_DARWIN)
#define LIB_EXTENSION_NAME "dylib"
#else
#define LIB_EXTENSION_NAME "so"
#endif

#define LIB_SUFFIX "." LIB_EXTENSION_NAME

Enjam::utils::Path Enjam::utils::libPath(const Enjam::utils::Path& path, const std::string& name) {
  return path / Path { LIB_PREFIX + name + LIB_SUFFIX };
}

Enjam::utils::Path Enjam::utils::getTempFilePath(const Enjam::utils::Path& directory, const Enjam::utils::Path& filename) {
  const std::time_t now = std::time(nullptr);
  return directory / Path { std::to_string(now) + filename.string() };
}

void* Enjam::utils::loadLib(const Enjam::utils::Path& path) {

#ifdef ENJAM_PLATFORM_WINDOWS

#else
  void* dllHandle = ::dlopen(path.c_str(), RTLD_LAZY);
#endif
  return (void*) dllHandle;
}

void Enjam::utils::freeLib(void* lib) {
  if (lib == nullptr) {
    return;
  }

#ifdef ENJAM_PLATFORM_WINDOWS

#else
  ::dlclose(lib);
#endif
}

void* Enjam::utils::getProcAddress(void* lib, const std::string &name) {
#ifdef ENJAM_PLATFORM_WINDOWS

#else
  return ::dlsym(lib, name.c_str());
#endif
}