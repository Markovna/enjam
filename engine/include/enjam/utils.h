#ifndef INCLUDE_ENJAM_UTILS_H_
#define INCLUDE_ENJAM_UTILS_H_

#include <string>

namespace Enjam::utils {

std::string libPath(const std::string& path, const std::string& name);

void* loadLib(const std::string &path);
void freeLib(void*);
void* getProcAddress(void*, const std::string& name);

}

#endif //INCLUDE_ENJAM_UTILS_H_
