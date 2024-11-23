add_library(enjam STATIC
        src/Test.cpp
        src/engine.cpp
        src/input.cpp
        src/platform_glfw.cpp
        src/platform_mac.cpp
        src/platform_win.cpp
        src/library_loader.cpp)

target_compile_definitions(enjam PRIVATE ENJAM_EXPORT)
target_include_directories(enjam PUBLIC include)

target_link_libraries(enjam
        fmt
        glad
        glfw)