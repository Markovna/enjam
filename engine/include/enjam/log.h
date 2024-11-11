#pragma once

#include <enjam/enjam.h>
#include <fmt/format.h>
#include <iostream>

namespace Enjam {

class ENJAM_API Log {
 public:
  template<typename... Args> static void info(const char* location, fmt::format_string<Args...> format, Args&&... args);
  template<typename... Args> static void debug(const char* location, fmt::format_string<Args...> format, Args&&... args);
  template<typename... Args> static void warn(const char* location, fmt::format_string<Args...> format, Args&&... args);
  template<typename... Args> static void error(const char* location, fmt::format_string<Args...> format, Args&&... args);
};

template<typename... Args>
void Log::info(const char* location, fmt::format_string<Args...> format, Args &&... args) {
  std::cout << "[INFO] " << fmt::format(format, args...) << " (" << location << ")\n";
}

template<typename... Args>
void Log::debug(const char* location, fmt::format_string<Args...> format, Args &&... args) {
  std::cout << "[DEBUG] " << fmt::format(format, args...) << " (" << location << ")\n";
}

template<typename... Args>
void Log::warn(const char* location, fmt::format_string<Args...> format, Args &&... args) {
  std::cout << "[WARN] " << fmt::format(format, args...) << " (" << location << ")\n";
}

template<typename... Args>
void Log::error(const char* location, fmt::format_string<Args...> format, Args &&... args) {
  std::cout << "[ERROR] " << fmt::format(format, args...) << " (" << location << ")\n";
}

}

#define EJ_LOG_S1(x) #x
#define EJ_LOG_S2(x) EJ_LOG_S1(x)
#define EJ_LOG_LOCATION __FILE__ ":" EJ_LOG_S2(__LINE__)

#define EJ_INFO(format, ...) Log::info(EJ_LOG_LOCATION, format, ##__VA_ARGS__)
#define EJ_DEBUG(format, ...) Log::debug(EJ_LOG_LOCATION, format, ##__VA_ARGS__)
#define EJ_WARN(format, ...) Log::warn(EJ_LOG_LOCATION, format, ##__VA_ARGS__)
#define EJ_ERROR(format, ...) Log::error(EJ_LOG_LOCATION, format, ##__VA_ARGS__)