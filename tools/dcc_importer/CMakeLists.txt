cmake_minimum_required(VERSION 3.24)
set(CMAKE_CXX_STANDARD 17)

project(dcc_importer)
add_executable(dcc_importer src/dcc_importer.cpp)

target_link_libraries(dcc_importer PRIVATE enjam)
target_link_libraries(dcc_importer PRIVATE assimp)