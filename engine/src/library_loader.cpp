#include <enjam/library_loader.h>
#include <enjam/log.h>
#include <enjam/defines.h>
#include <enjam/utils.h>

namespace Enjam {

Library::Library(const std::string& path)
  : path(path), handle(nullptr) {

}

Library::~Library() {
  if(handle != nullptr) {
    unload();
  }
}

bool Library::load() {
  handle = utils::loadLib(path);
  if(!handle) {
    ENJAM_ERROR("Loading dll at path {} failed", path);
    return false;
  }
  return true;
}

void Library::unload() {
  if(handle == nullptr) {
    return;
  }

  utils::freeLib(handle);
  handle = nullptr;
}

void* Library::getProcAddress(const std::string& name) const {
  return utils::getProcAddress(handle, name);
}

}