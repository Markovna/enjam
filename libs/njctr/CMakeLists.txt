cmake_minimum_required(VERSION 3.24)
set(CMAKE_CXX_STANDARD 17)

project(njctr)
add_library(njctr INTERFACE include/njctr/njctr.h)
target_include_directories(njctr INTERFACE include)

project(njctr_tests)
add_executable(njctr_tests tests/common_tests.cpp)
target_link_libraries(njctr_tests njctr)