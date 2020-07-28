add_library(glad STATIC
        glad.c
        include/glad.h)

add_library(glad::glad ALIAS glad)

target_compile_features(glad PRIVATE c_std_99)
target_include_directories(glad
PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)
