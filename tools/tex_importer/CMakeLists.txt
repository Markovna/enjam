cmake_minimum_required(VERSION 3.24)
set(CMAKE_CXX_STANDARD 17)

project(tex_importer)
add_executable(tex_importer src/tex_importer.cpp)

target_link_libraries(tex_importer PRIVATE enjam)
target_link_libraries(tex_importer PRIVATE stb_image)