add_library(stb_image STATIC
        stb_image.c
        include/stb_image.h)

add_library(stb_image::stb_image ALIAS stb_image)

target_compile_features(stb_image PRIVATE c_std_99)
target_include_directories(stb_image
PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)
