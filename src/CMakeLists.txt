add_library(sig8 STATIC
        ../include/sig8.h
        util.c
        render.c
        sig8_internal.h
        graphics.c
        resources/font5x7.c
        resources/font3x5.c
        window.c
        input.c
        stb_image.c
        stb_image.h
        resources/font_aseprite.c audio.c music.c)

set_target_properties(sig8 PROPERTIES VERSION ${PROJECT_VERSION})
target_compile_features(sig8 PUBLIC c_std_11)
find_package (Threads)

target_include_directories(sig8
PUBLIC
    "${CMAKE_SOURCE_DIR}/include/"
PRIVATE
    "${CMAKE_SOURCE_DIR}/src/"
    SDL2
    OpenGL::GL
)

target_link_libraries(sig8
PRIVATE
    SDL2
    OpenGL::GL
    ${CMAKE_DL_LIBS}
    Threads::Threads
    m
)
