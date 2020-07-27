add_library(stb_image_write STATIC
        stb_image_write.c
        include/stb_image_write.h)

add_library(stb_image_write::stb_image_write ALIAS stb_image_write)

target_compile_features(stb_image_write PRIVATE c_std_99)
target_include_directories(stb_image_write
PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)
