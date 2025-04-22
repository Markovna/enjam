cmake_minimum_required(VERSION 3.24)
set(CMAKE_CXX_STANDARD 17)

project(enjam)

add_subdirectory(external/fmt EXCLUDE_FROM_ALL)
add_subdirectory(external/glad EXCLUDE_FROM_ALL)
add_subdirectory(external/glfw EXCLUDE_FROM_ALL)
add_subdirectory(external/stb_image EXCLUDE_FROM_ALL)
add_subdirectory(external/assimp EXCLUDE_FROM_ALL)
add_subdirectory(libs/njctr EXCLUDE_FROM_ALL)
add_subdirectory(libs/vulkan EXCLUDE_FROM_ALL)

add_subdirectory(engine)
add_subdirectory(editor)

add_subdirectory(tools/tex_importer)
add_subdirectory(tools/dcc_importer)

add_subdirectory(sandbox)