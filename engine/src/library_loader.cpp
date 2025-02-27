#include <enjam/library_loader.h>
#include <enjam/log.h>
#include <enjam/utils.h>

#include <utility>

namespace Enjam {

Library::Library(Path  path)
  : path(std::move(path)), handle(nullptr) {

}

Library::Library(Library&& other)
  : path(std::move(other.path))
  , handle(std::exchange(other.handle, nullptr)) {
}

Library& Library::operator=(Library&& other) noexcept {
  if(handle) {
    unload();
  }

  handle = std::exchange(other.handle, nullptr);
  path = std::move(other.path);

  return *this;
}

Library::~Library() {
  if(handle) {
    unload();
  }
}

bool Library::load() {
  handle = utils::loadLib(path);
  if(!handle) {
    ENJAM_ERROR("Loading dll at path {} failed", path.string());
    return false;
  }
  return true;
}

void Library::unload() {
  if(!handle) {
    return;
  }

  utils::freeLib(handle);
  handle = nullptr;
}

void* Library::getProcAddress(const std::string& name) const {
  return utils::getProcAddress(handle, name);
}

LibraryLoader::LibraryLoader(Path path) : path(std::move(path)) {

}

void LibraryLoader::load(const Path& libPath, Context& context) {
  auto tmpFilePath = utils::getTempFilePath(path, libPath.filename());
  bool copied = copy_file(libPath, tmpFilePath);
  if(!copied) {
    ENJAM_ERROR("Failed to copy file {} to {}", libPath.string(), tmpFilePath.string());
    return;
  }

  Library lib = Library { tmpFilePath };
  load(lib, context);

  auto it = libs.find(libPath.filename());
  if(it != libs.end()) {
    unload(it->second, context);
    it->second = std::move(lib);
  } else {
    it = libs.emplace(libPath.filename(), std::move(lib)).first;
  }
}

void LibraryLoader::unload(const Path& libPath, Context& context) {
  auto it = libs.find(libPath.filename());
  if(it != libs.end()) {
    unload(it->second, context);
  }

  libs.erase(it);
}


void LibraryLoader::load(Library& lib, Context& context) {
  const std::string funcName = "loadLib";
  bool loaded = lib.load();
  if(!loaded) {
    ENJAM_ERROR("Failed to load lib at path: {}", lib.getPath().string());
    return;
  }

  auto funcPtr = reinterpret_cast<LoadFunc>(lib.getProcAddress(funcName));
  if(!funcPtr) {
    ENJAM_ERROR("Failed to find func {} in {}", funcName, lib.getPath().string());
    return;
  }

  funcPtr(context);
}

void LibraryLoader::unload(Library& lib, Context& context) {
  const std::string funcName = "unloadLib";
  auto funcPtr = reinterpret_cast<UnloadFunc>(lib.getProcAddress(funcName));
  if(!funcPtr) {
    ENJAM_ERROR("Failed to find func {} in {}", funcName, lib.getPath().string());
    lib.unload();
    return;
  }

  funcPtr(context);
  lib.unload();
}

}