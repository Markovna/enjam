set(CMAKE_DEBUG_POSTFIX "")

set(SHADERS
        shaders/simple.vert
        shaders/simple.frag)

set(TEXTURES
        textures/dummy.png
        )

set(MODELS
        models/cube.obj
        )

set(ASSETS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/assets)
set(ASSETS_DIR_BIN ${CMAKE_CURRENT_BINARY_DIR}/assets)

add_custom_target(copy_assets
        COMMAND ${CMAKE_COMMAND} -E make_directory ${ASSETS_DIR_BIN}
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${ASSETS_DIR} ${ASSETS_DIR_BIN}
        COMMENT "Copying ${ASSETS_DIR} to ${ASSETS_DIR_BIN}..."
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        )

set(ASSETS)
foreach(resource ${TEXTURES})
    get_filename_component(local_name "${resource}" NAME_WE)
    get_filename_component(input_path "${resource}" ABSOLUTE)

    set(output_path "${ASSETS_DIR}/textures/${local_name}.nj_tex")

    add_custom_command(
        OUTPUT ${output_path}
        COMMAND tex_importer -o ${output_path} ${input_path}
        MAIN_DEPENDENCY ${resource}
        DEPENDS tex_importer
        COMMENT "Generating asset file ${resource} in ${output_path}"
    )
    list(APPEND ASSETS ${output_path})
endforeach()

foreach(resource ${MODELS})
    get_filename_component(local_name "${resource}" NAME_WE)
    get_filename_component(input_path "${resource}" ABSOLUTE)

    set(output_path "${ASSETS_DIR}/models/${local_name}.nj_dcc")

    add_custom_command(
            OUTPUT ${output_path}
            COMMAND dcc_importer -o ${output_path} ${input_path}
            MAIN_DEPENDENCY ${resource}
            DEPENDS dcc_importer
            COMMENT "Generating asset file ${resource} in ${output_path}"
    )
    list(APPEND ASSETS ${output_path})
endforeach()

foreach (resource ${SHADERS})
    set(output_path "${CMAKE_CURRENT_BINARY_DIR}/${resource}")
    get_filename_component(directory "${output_path}" DIRECTORY)
    file(COPY ${resource} DESTINATION "${directory}")
    list(APPEND SHADER_BINS ${output_path})
endforeach()

file(COPY ${ASSETS_DIR} DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")

# Sandbox executable
add_executable(sandbox src/main.cpp)
set_property(TARGET sandbox PROPERTY ENABLE_EXPORTS ON)
target_link_libraries(sandbox enjam)

# Sandbox shared lib
add_library(game SHARED src/game.cpp ${ASSETS} ${SHADER_BINS})
add_dependencies(game copy_assets)
target_link_libraries(game enjam)