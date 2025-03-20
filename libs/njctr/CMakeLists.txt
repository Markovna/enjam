cmake_minimum_required(VERSION 3.24)
set(CMAKE_CXX_STANDARD 17)

project(njctr)
add_library(njctr STATIC include/njctr/njctr.h src/njctr.cpp)
target_include_directories(njctr PUBLIC include)
set_target_properties(njctr PROPERTIES LINKER_LANGUAGE CXX)

project(njctr_tests)
add_executable(njctr_tests tests/common_tests.cpp)
target_link_libraries(njctr_tests njctr)