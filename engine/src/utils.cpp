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

std::string Enjam::utils::libPath(const std::string& path, const std::string& name) {
  return path + "/" + LIB_PREFIX + name + LIB_SUFFIX;
}

void* Enjam::utils::loadLib(const std::string &path) {

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