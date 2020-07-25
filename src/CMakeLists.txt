set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")

add_library(sig8 STATIC
        ../include/sig8.h
        util.c
        sig8_internal.h
        graphics.c
        resources/font5x7.c
        resources/font3x5.c
        window.c
        input.c
        stb_image.c
        stb_image.h
        resources/font_aseprite.c
        audio.c
        music.c
        alloc.c
        events.c
        tilemap.c
)


if( ${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
    target_compile_options(sig8 PUBLIC -sUSE_SDL=2)
    target_link_options(sig8 PUBLIC -sUSE_SDL=2 -sFULL_ES3=1)
else()
    find_package(SDL2 REQUIRED)
    find_package(OpenGL REQUIRED)

    target_include_directories(sig8 PRIVATE SDL2 OpenGL::GL)
    target_link_libraries(sig8 PRIVATE SDL2 OpenGL::GL)
endif()

set_target_properties(sig8 PROPERTIES VERSION ${PROJECT_VERSION})
target_compile_features(sig8 PUBLIC c_std_11)

target_include_directories(sig8
PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/../include/"
PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/"
)

target_link_libraries(sig8
PRIVATE
    ${CMAKE_DL_LIBS}
    m
)
