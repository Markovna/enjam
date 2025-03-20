# Sandbox executable
add_executable(sandbox src/main.cpp)
set_property(TARGET sandbox PROPERTY ENABLE_EXPORTS ON)
target_link_libraries(sandbox enjam)

# Sandbox shared lib
add_library(game SHARED src/game.cpp)
target_link_libraries(game enjam)

set(RESOURCES
        textures/dummy.png
        textures/dummy.jpg
        shaders/vertex.glsl
        shaders/fragment.glsl)

foreach (resource ${RESOURCES})
    set(output_path "${CMAKE_CURRENT_BINARY_DIR}/${resource}")
    configure_file(${resource} ${output_path} COPYONLY)
#    list(APPEND SHADER_BINS ${output_path})
endforeach()

