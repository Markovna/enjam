add_executable(editor src/main.cpp)
set_property(TARGET editor PROPERTY ENABLE_EXPORTS ON)

target_link_libraries(editor enjam)