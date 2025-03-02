#pragma once

#include <enjam/utils.h>
#include <string>
#include <filesystem>
#include <unordered_map>

namespace Enjam {

class Context;

class Library {
 public:
  using Path = utils::Path;

  Library(Path path);
  Library(const Library&) = delete;
  Library& operator=(const Library&) = delete;

  Library(Library&&);
  Library& operator=(Library&&) noexcept;

  ~Library();

  bool load();
  void unload();
  void* getProcAddress(const std::string& name) const;

  bool isLoaded() const { return handle != nullptr; }
  const Path& getPath() const { return path; }

 private:
  void* handle;
  Path path;
};


class LibraryLoader {
 public:
  using LoadCallback = std::function<void(Library&)>;
  using UnloadCallback = std::function<void(Library&)>;
  using Path = utils::Path;

  explicit LibraryLoader(Path path, LoadCallback, UnloadCallback);
  ~LibraryLoader() = default;

  LibraryLoader(const LibraryLoader&) = delete;
  LibraryLoader(LibraryLoader&&) = delete;
  LibraryLoader& operator=(const LibraryLoader&) = delete;
  LibraryLoader& operator=(LibraryLoader&&) = delete;

  void load(const Path&);
  void unload(const Path&);
  void clear();

 private:
  void load(Library& lib);
  void unload(Library& lib);

 private:
  Path path;
  LoadCallback mLoad;
  UnloadCallback mUnload;
  std::unordered_map<std::string, Library> libs;
};

}