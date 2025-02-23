add_executable(sandbox src/sandbox.cpp)
set_property(TARGET sandbox PROPERTY ENABLE_EXPORTS ON)

target_link_libraries(sandbox enjam)

add_library(game SHARED src/game.cpp)
target_link_libraries(game enjam)

set(SHADERS
        shaders/vertex.glsl
        shaders/fragment.glsl)

foreach (shader_src ${SHADERS})
    set(output_path "${CMAKE_CURRENT_BINARY_DIR}/${shader_src}")
    configure_file(${shader_src} ${output_path} COPYONLY)
#    list(APPEND SHADER_BINS ${output_path})
endforeach()
