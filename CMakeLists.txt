cmake_minimum_required(VERSION 3.24)
set(CMAKE_CXX_STANDARD 17)

project(enjam)

add_subdirectory(external EXCLUDE_FROM_ALL)
add_subdirectory(engine)
add_subdirectory(sandbox)