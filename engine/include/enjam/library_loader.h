#pragma once

#include <string>

namespace Enjam {

class Library {
 public:
  Library(const std::string& path);
  ~Library();

  bool load();
  void unload();
  void* getProcAddress(const std::string& name) const;

  bool isLoaded() const { return handle != nullptr; }
  const std::string& getPath() const { return path; }

 private:
  void* handle;
  std::string path;
};


class LibraryLoader {
 public:
  LibraryLoader() = default;
  ~LibraryLoader() = default;

  LibraryLoader(const LibraryLoader&) = delete;
  LibraryLoader(LibraryLoader&&) = delete;
  LibraryLoader& operator=(const LibraryLoader&) = delete;
  LibraryLoader& operator=(LibraryLoader&&) = delete;
};

}