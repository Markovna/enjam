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
  using Path = utils::Path;

  explicit LibraryLoader(Path path);
  ~LibraryLoader() = default;

  LibraryLoader(const LibraryLoader&) = delete;
  LibraryLoader(LibraryLoader&&) = delete;
  LibraryLoader& operator=(const LibraryLoader&) = delete;
  LibraryLoader& operator=(LibraryLoader&&) = delete;

  void load(const Path&, Context& context);
  void unload(const Path&, Context& context);
  void clear();

 private:
  void load(Library& lib, Context& context);
  void unload(Library& lib, Context& context);

 private:
  typedef void (*LoadFunc)(Context& context);
  typedef void (*UnloadFunc)(Context& context);

  Path path;
  std::unordered_map<std::string, Library> libs;
};

}