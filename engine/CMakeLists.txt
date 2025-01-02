set(ENJAM_SOURCES
        src/Test.cpp
        src/input.cpp
        src/platform_glfw.cpp
        src/platform_mac.cpp
        src/platform_win.cpp
        src/library_loader.cpp
        src/renderer.cpp
        src/renderer_backend_opengl.cpp
)

set(ENJAM_HEADERS
        include/enjam/assert.h
        include/enjam/defines.h
        include/enjam/event.h
        include/enjam/handle_allocator.h
        include/enjam/input.h
        include/enjam/input_events.h
        include/enjam/library_loader.h
        include/enjam/log.h
        include/enjam/platform.h
        include/enjam/platform_glfw.h
        include/enjam/platform_mac.h
        include/enjam/platform_win.h
        include/enjam/renderer.h
        include/enjam/renderer_backend.h
        include/enjam/renderer_backend_opengl.h
        include/enjam/renderer_backend_type.h
        include/enjam/Test.h
)

add_library(enjam STATIC ${ENJAM_SOURCES} ${ENJAM_HEADERS})

target_include_directories(enjam PUBLIC include)

target_link_libraries(enjam PUBLIC fmt)
target_link_libraries(enjam PRIVATE glfw)
target_link_libraries(enjam PRIVATE glad)