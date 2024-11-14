#pragma once

#ifdef ENJAM_PLATFORM_WINDOWS
  #define ENJAM_SYMBOL_EXPORT __declspec(dllexport)
  #define ENJAM_SYMBOL_IMPORT __declspec(dllimport)

#else
  #define ENJAM_SYMBOL_EXPORT __attribute__((visibility("default")))
  #define ENJAM_SYMBOL_IMPORT

#endif

#ifdef ENJAM_EXPORT
  #define ENJAM_API ENJAM_SYMBOL_EXPORT
#else
  #define ENJAM_API ENJAM_SYMBOL_IMPORT
#endif