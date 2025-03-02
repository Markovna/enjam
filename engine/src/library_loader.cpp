#include <enjam/library_loader.h>
#include <enjam/log.h>
#include <enjam/utils.h>

#include <utility>

namespace Enjam {

Library::Library(Path path)
  : path(std::move(path)), handle(nullptr) {

}

Library::Library(Library&& other)
  : path(std::move(other.path))
  , handle(std::exchange(other.handle, nullptr)) {
}

Library& Library::operator=(Library&& other) noexcept {
  if(handle && handle != other.handle) {
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

LibraryLoader::LibraryLoader(Path path, LoadCallback load, UnloadCallback unload)
: path(std::move(path)), mLoad(load), mUnload(unload) {

}

void LibraryLoader::load(const Path& libPath) {
  auto tmpFilePath = utils::getTempFilePath(path, libPath.filename());
  bool copied = copy_file(libPath, tmpFilePath);
  if(!copied) {
    ENJAM_ERROR("Failed to copy file {} to {}", libPath.string(), tmpFilePath.string());
    return;
  }

  Library lib = Library { tmpFilePath };
  load(lib);

  auto it = libs.find(libPath.filename());
  if(it != libs.end()) {
    unload(it->second);
    it->second = std::move(lib);
  } else {
    it = libs.emplace(libPath.filename(), std::move(lib)).first;
  }
}

void LibraryLoader::unload(const Path& libPath) {
  auto it = libs.find(libPath.filename());
  if(it != libs.end()) {
    unload(it->second);
    libs.erase(it);
  }
}


void LibraryLoader::load(Library& lib) {
  bool loaded = lib.load();
  if(!loaded) {
    ENJAM_ERROR("Failed to load lib at path: {}", lib.getPath().string());
    return;
  }

  mLoad(lib);
}

void LibraryLoader::unload(Library& lib) {
  mUnload(lib);
  lib.unload();
}

}