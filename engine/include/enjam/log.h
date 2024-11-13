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

#define __ENJAM_LOG_S1(x) #x
#define __ENJAM_LOG_S2(x) __ENJAM_LOG_S1(x)
#define __ENJAM_LOG_LOCATION __FILE__ ":" __ENJAM_LOG_S2(__LINE__)

#define ENJAM_INFO(format, ...) Enjam::Log::info(__ENJAM_LOG_LOCATION, format, ##__VA_ARGS__)
#define ENJAM_DEBUG(format, ...) Enjam::Log::debug(__ENJAM_LOG_LOCATION, format, ##__VA_ARGS__)
#define ENJAM_WARN(format, ...) Enjam::Log::warn(__ENJAM_LOG_LOCATION, format, ##__VA_ARGS__)
#define ENJAM_ERROR(format, ...) Enjam::Log::error(__ENJAM_LOG_LOCATION, format, ##__VA_ARGS__)