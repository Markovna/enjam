#pragma once

#ifdef EJEXPORT
#ifdef _MSC_VER
#define ENJAM_API __declspec(dllexport) // Microsoft
#elif defined(__GNUC__)
#define ENJAM_API __attribute__((visibility("default"))) // GCC
#else
#define ENJAM_API // Most compilers export all the symbols by default. We hope for the best here.
    #pragma warning Unknown dynamic link import/export semantics.
#endif
#else // Imports
#ifdef _MSC_VER
#define ENJAM_API __declspec(dllimport) // Microsoft
#else
#define ENJAM_API
#pragma warning Unknown dynamic link import/export semantics.
#endif
#endif