#pragma once

#include <string>

class LibraryLoader final {
 public:
  LibraryLoader();
  ~LibraryLoader();

  bool load(const std::string& path, const std::string& name);
  void free();
  void* getProcAddress(const std::string& name) const;

 private:
  void* dllHandle;
};
