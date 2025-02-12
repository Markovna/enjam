#pragma once

#include <string>

namespace Enjam {

class Library;

class LibraryLoader {
 public:
  LibraryLoader();
  ~LibraryLoader();

  LibraryLoader(LibraryLoader&) = delete;
  LibraryLoader(LibraryLoader&&) = delete;

  Library* load(const std::string &path, const std::string &name);
  void free(Library*);
  void* getProcAddress(Library*, const std::string& name) const;
};

}