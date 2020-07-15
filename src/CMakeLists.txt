add_library(sig8 STATIC
        ../include/sig8.h
        util.c
        render.c
        sig8_internal.h
        glad.h
        glad.c
        graphics.c
        resources/font5x7.c
        resources/font3x5.c
        window.c
        input.c)

set_target_properties(sig8 PROPERTIES VERSION ${PROJECT_VERSION})
target_compile_features(sig8 PUBLIC c_std_11)

target_include_directories(sig8
PUBLIC
    "${CMAKE_SOURCE_DIR}/include/"
PRIVATE
    "${CMAKE_SOURCE_DIR}/src/"
    SDL2
    OpenGL::GL
)

target_link_libraries(sig8
PUBLIC
    SDL2
    OpenGL::GL
    ${CMAKE_DL_LIBS}
)