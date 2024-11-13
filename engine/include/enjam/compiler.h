#pragma once

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
