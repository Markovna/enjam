#ifndef INCLUDE_ENJAM_UTILS_H_
#define INCLUDE_ENJAM_UTILS_H_

#include <enjam/assert.h>
#include <string>
#include <filesystem>

namespace Enjam::utils {

using Path = std::filesystem::path;

Path libPath(const Path& path, const std::string& name);
Path getTempFilePath(const Path& directory, const Path& filename);

void* loadLib(const Path& path);
void freeLib(void*);
void* getProcAddress(void*, const std::string& name);

template<class T, std::enable_if_t<std::is_trivially_copyable<T>::value, nullptr_t> = nullptr>
std::vector<T> reinterpret_copy(const void* src, size_t size) {
  ENJAM_ASSERT(size % sizeof(T) == 0);
  std::vector<T> dst(size / sizeof(T));
  std::copy_n(reinterpret_cast<const T*>(src), dst.size(), dst.begin());
  return dst;
}

}

#endif //INCLUDE_ENJAM_UTILS_H_
