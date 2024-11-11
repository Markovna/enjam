add_library(enjam SHARED src/Test.cpp)

target_compile_definitions(enjam PRIVATE EJEXPORT)
target_include_directories(enjam PUBLIC include)

target_link_libraries(enjam fmt)