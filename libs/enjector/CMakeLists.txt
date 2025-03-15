cmake_minimum_required(VERSION 3.24)
set(CMAKE_CXX_STANDARD 17)

project(enjector)
add_library(enjector STATIC include/enjector/enjector.h src/enjector.cpp include/enjector/factories.h)
target_include_directories(enjector PUBLIC include)
set_target_properties(enjector PROPERTIES LINKER_LANGUAGE CXX)

project(enjector_tests)
add_executable(enjector_tests tests/tests.cpp)
target_link_libraries(enjector_tests enjector)