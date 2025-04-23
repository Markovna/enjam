set(ENJAM_SOURCES
        src/input.cpp
        src/platform_glfw.cpp
        src/program.cpp
        src/platform_win.cpp
        src/library_loader.cpp
        src/renderer.cpp
        src/renderer_backend_opengl.cpp
        src/render_primitive.cpp
        src/render_view.cpp
        src/scene.cpp
        src/platform.cpp
        src/utils.cpp
        src/application.cpp
        src/assets_repository.cpp
        src/assetfile_reader.cpp
        src/asset.cpp
        src/byte_array.cpp
        src/renderer_backend_vulkan.cpp)

set(ENJAM_HEADERS
        include/enjam/assert.h
        include/enjam/defines.h
        include/enjam/event.h
        include/enjam/handle_allocator.h
        include/enjam/input.h
        include/enjam/input_events.h
        include/enjam/library_loader.h
        include/enjam/log.h
        include/enjam/program.h
        include/enjam/platform.h
        include/enjam/platform_glfw.h
        include/enjam/platform_win.h
        include/enjam/renderer.h
        include/enjam/renderer_backend.h
        include/enjam/renderer_backend_opengl.h
        include/enjam/renderer_backend_type.h
        include/enjam/render_primitive.h
        include/enjam/scene.h
        include/enjam/utils.h
        include/enjam/dependencies.h
        include/enjam/application.h
        include/enjam/simulation.h
        include/enjam/material.h
        include/enjam/asset.h
        include/enjam/assetfile_reader.h
        include/enjam/assets_repository.h
        include/enjam/assetfile_writer.h
        include/enjam/assets_manager.h
        include/enjam/texture.h
        include/enjam/dcc_asset.h
        include/enjam/math_assetparser.h
        include/enjam/byte_array.h include/enjam/renderer_backend_vulkan.h include/enjam/vulkan_defines.h)

find_package(Vulkan REQUIRED)

add_library(enjam SHARED ${ENJAM_SOURCES} ${ENJAM_HEADERS})
target_include_directories(enjam PUBLIC include)

target_link_libraries(enjam PUBLIC fmt)
target_link_libraries(enjam PRIVATE glfw)
#target_link_libraries(enjam PUBLIC stb_image)
target_link_libraries(enjam PRIVATE glad)
target_link_libraries(enjam PUBLIC njctr)
target_link_libraries(enjam PRIVATE Vulkan::Vulkan)
target_link_libraries(enjam PRIVATE vulkan)

add_subdirectory(tests EXCLUDE_FROM_ALL)