#ifndef INCLUDE_ENJAM_UTILS_H_
#define INCLUDE_ENJAM_UTILS_H_

#include <string>
#include <filesystem>

namespace Enjam::utils {

using Path = std::filesystem::path;

Path libPath(const Path& path, const std::string& name);
Path getTempFilePath(const Path& directory, const Path& filename);

void* loadLib(const Path& path);
void freeLib(void*);
void* getProcAddress(void*, const std::string& name);

}

#endif //INCLUDE_ENJAM_UTILS_H_
