set(SHADERS
        shaders/vertex.glsl
        shaders/fragment.glsl)

set(TEXTURES
        textures/dummy.png
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
foreach (tex ${TEXTURES})
    get_filename_component(localname "${tex}" NAME_WE)
    get_filename_component(fullname "${tex}" ABSOLUTE)

    set(output_path "${ASSETS_DIR}/textures/${localname}.nj_tex")
    #get_filename_component(full_output ${output_path} ABSOLUTE)

    add_custom_command(
        OUTPUT ${output_path}
        COMMAND njtex -o ${output_path} ${fullname}
        MAIN_DEPENDENCY ${tex}
        DEPENDS njtex
        COMMENT "Generating asset file ${tex} to ${output_path}"
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
add_executable(sandbox src/main.cpp ${ASSETS} ${SHADER_BINS})
add_dependencies(sandbox copy_assets)
set_property(TARGET sandbox PROPERTY ENABLE_EXPORTS ON)
target_link_libraries(sandbox enjam)

# Sandbox shared lib
add_library(game SHARED src/game.cpp)
target_link_libraries(game enjam)