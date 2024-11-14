add_library(enjam STATIC src/Test.cpp src/library_loader.cpp src/window_mock.cpp)

target_compile_definitions(enjam PRIVATE ENJAM_EXPORT)
target_include_directories(enjam PUBLIC include)

target_link_libraries(enjam fmt)