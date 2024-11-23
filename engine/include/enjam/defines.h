#pragma once

// --- Compiler Defines ---
#define ENJAM_COMPILER_GCC 0
#define ENJAM_COMPILER_CLANG 0
#define ENJAM_COMPILER_MSVC 0

#if defined(__GNUC__)
#   undef ENJAM_COMPILER_GCC
#   define ENJAM_COMPILER_GCC ((__GNUC__ * 10000) + (__GNUC_MINOR__ * 100) + (__GNUC_PATCHLEVEL__))
#elif defined(__clang__)
#   undef  ENJAM_COMPILER_CLANG
#   define ENJAM_COMPILER_CLANG ((__clang_major__ * 10000) + (__clang_minor__ * 100) + (__clang_patchlevel__))
#elif defined(_MSC_VER)
#   undef  ENJAM_COMPILER_MSVC
#   define ENJAM_COMPILER_MSVC _MSC_VER
#else
#	error "Unknown/unsupported compiler"
#endif

// --- Platform Defines ---
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

// --- API Defines ---
#ifdef ENJAM_PLATFORM_WINDOWS
  #define __ENJAM_SYMBOL_EXPORT __declspec(dllexport)
  #define __ENJAM_SYMBOL_IMPORT __declspec(dllimport)

#else
  #define __ENJAM_SYMBOL_EXPORT __attribute__((visibility("default")))
  #define __ENJAM_SYMBOL_IMPORT

#endif

#ifdef ENJAM_EXPORT
  #define ENJAM_API __ENJAM_SYMBOL_EXPORT
#else
  #define ENJAM_API __ENJAM_SYMBOL_IMPORT
#endif