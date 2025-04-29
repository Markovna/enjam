cmake_minimum_required(VERSION 3.24)
set(CMAKE_CXX_STANDARD 17)

project(shader_importer)
add_executable(shader_importer src/shader_importer.cpp)

target_link_libraries(shader_importer PRIVATE enjam)
target_link_libraries(shader_importer PRIVATE glslang)
target_link_libraries(shader_importer PRIVATE glslang-default-resource-limits)