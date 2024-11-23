add_library(glad EXCLUDE_FROM_ALL include/glad/glad.h src/glad.c)
target_include_directories(glad PUBLIC include/)