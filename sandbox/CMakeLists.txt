add_executable(sandbox src/sandbox.cpp)

target_link_libraries(sandbox enjam)

add_library(game SHARED src/game.cpp)
target_link_libraries(game enjam)