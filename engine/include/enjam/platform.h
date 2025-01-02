#ifndef INCLUDE_ENJAM_PLATFORM_H_
#define INCLUDE_ENJAM_PLATFORM_H_

#include <enjam/defines.h>

#if defined(ENJAM_PLATFORM_WINDOWS)
#include <enjam/platform_glfw.h>

#elif defined(ENJAM_PLATFORM_DARWIN)
#include <enjam/platform_glfw.h>

#endif

#endif //INCLUDE_ENJAM_PLATFORM_H_