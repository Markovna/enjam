set(ENJAM_SOURCES
        src/input.cpp
        src/platform_glfw.cpp
        src/program.cpp
        src/platform_mac.cpp
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
        src/assetfile_parser.cpp src/asset_manager.cpp)

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
        include/enjam/platform_mac.h
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
        include/enjam/assetfile_parser.h include/enjam/asset_manager.h)

add_library(enjam STATIC ${ENJAM_SOURCES} ${ENJAM_HEADERS})

target_include_directories(enjam PUBLIC include)

target_link_libraries(enjam PUBLIC fmt)
target_link_libraries(enjam PRIVATE glfw)
target_link_libraries(enjam PUBLIC stb_image)
target_link_libraries(enjam PRIVATE glad)
target_link_libraries(enjam PUBLIC njctr)

add_subdirectory(test EXCLUDE_FROM_ALL)