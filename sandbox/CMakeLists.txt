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

set(TEXTURES
        textures/dummy.png
        )

foreach (resource ${RESOURCES})
    set(output_path "${CMAKE_CURRENT_BINARY_DIR}/${resource}")
    configure_file(${resource} ${output_path} COPYONLY)
#    list(APPEND SHADER_BINS ${output_path})
endforeach()

set(ASSETS_DIR ${CMAKE_SOURCE_DIR}/assets)

foreach (tex ${TEXTURES})
    get_filename_component(localname "${tex}" NAME_WE)
    get_filename_component(fullname "${tex}" ABSOLUTE)
    set(output_path "${ASSETS_DIR}/textures/${localname}.nj_tex")
    add_custom_command(
            OUTPUT ${output_path}
            COMMAND njtex -o ${output_path} ${fullname}
            MAIN_DEPENDENCY ${tex}
            DEPENDS njtex
            COMMENT "Generating asset file ${tex} to ${output_path}"
    )
endforeach()
