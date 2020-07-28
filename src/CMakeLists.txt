add_library(sig8 STATIC
        ../include/sig8.h
        util.c
        sig8_internal.h
        graphics.c
        resources/font5x7.c
        resources/font3x5.c
        window.c
        input.c
        resources/font_aseprite.c
        audio.c
        music.c
        alloc.c
        events.c
        tilemap.c
        fs.c
        resource.c)

add_library(sig8::sig8 ALIAS sig8)

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/config.h.in" "${CMAKE_CURRENT_BINARY_DIR}/config.h")

if (MSVC)
    target_compile_options(sig8 PRIVATE /W4)
else()
    target_compile_options(sig8 PRIVATE -Wall -Wextra)
endif()

if (${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
    target_compile_options(sig8 PUBLIC -sUSE_SDL=2)
    target_link_options(sig8 PUBLIC -sUSE_SDL=2 -sFULL_ES3=1)
else()
    find_package(SDL2 REQUIRED)
    find_package(OpenGL REQUIRED)

    target_include_directories(sig8 PRIVATE SDL2 OpenGL::GL)
    target_link_libraries(sig8 PRIVATE SDL2 OpenGL::GL glad)
endif()

if (SIG8_ENABLE_ASAN)
    target_compile_options(sig8 PUBLIC -fsanitize=address)
    target_link_options(sig8 PUBLIC -fsanitize=address)
endif()

if (SIG8_COMPILE_EDITORS)
    sig8_bundle(sig8
        FORCE
        NAME SIG8_EDITORS_BUNDLE
        FILE sig8_editors_resources
        RESOURCES editors/spritesheet.png
    )

    target_link_libraries(sig8 PRIVATE stb_image_write::stb_image_write)
    target_sources(sig8 PRIVATE
            editors/editors.h
            editors/history.c
            editors/sprite_editor.c
            editors/ui.c
            editors/tile_editor.c)
endif()

set_target_properties(sig8 PROPERTIES VERSION ${SIG8_VERSION})
target_compile_features(sig8 PRIVATE c_std_99)

target_include_directories(sig8
PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/../include/"
PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_BINARY_DIR}"
)

target_link_libraries(sig8
PRIVATE
    ${CMAKE_DL_LIBS}
    stb_image::stb_image
    m
)
