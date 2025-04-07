cmake_minimum_required(VERSION 3.24)
set(CMAKE_CXX_STANDARD 17)

project(njtex)
add_executable(njtex src/njtex.cpp)

target_link_libraries(njtex PRIVATE enjam)
target_link_libraries(njtex PRIVATE stb_image)