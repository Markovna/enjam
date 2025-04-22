cmake_minimum_required(VERSION 3.24)
set(CMAKE_CXX_STANDARD 17)

project(vulkan)

file(GLOB_RECURSE PUBLIC_HDRS include/vulkan/*.h)

add_library(vulkan INTERFACE ${PUBLIC_HDRS})
target_include_directories(vulkan INTERFACE include)