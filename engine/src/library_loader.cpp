#include <enjam/library_loader.h>
#include "enjam/defines.h"

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

#ifdef ENJAM_PLATFORM_WINDOWS
#include <windows.h>
#else
#include <dlfcn.h>
#endif

// TODO: windows platform

LibraryLoader::LibraryLoader()
  : dllHandle(nullptr) {

}

LibraryLoader::~LibraryLoader() {
  free();
}

bool LibraryLoader::load(const std::string& path, const std::string& name) {
  const std::string fullPath = path + "/" + LIB_PREFIX + name + LIB_SUFFIX;

#ifdef ENJAM_PLATFORM_WINDOWS

#else
  dllHandle = ::dlopen(fullPath.c_str(), RTLD_LAZY);
#endif
  return (dllHandle != nullptr);
}

void LibraryLoader::free() {
  if(dllHandle == nullptr) {
    return;
  }

#ifdef ENJAM_PLATFORM_WINDOWS

#else
  ::dlclose(dllHandle);
#endif

  dllHandle = nullptr;
}

void* LibraryLoader::getProcAddress(const std::string& name) const {
#ifdef ENJAM_PLATFORM_WINDOWS

#else
  return ::dlsym(dllHandle, name.c_str());
#endif
}
