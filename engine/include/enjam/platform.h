#pragma once

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
  #define ENJAM_PLATFORM_WINDOWS

#elif defined(linux) || defined(__linux) || defined(__linux__)
  #define ENJAM_PLATFORM_LINUX

#elif defined(__APPLE__)
  #include <TargetConditionals.h>
    #if TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR
      #define ENJAM_PLATFORM_IOS
    #else
      #define ENJAM_PLATFORM_DARWIN
    #endif
  #else
    #error "Unknown/unsupported platform"

#endif