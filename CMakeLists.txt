cmake_minimum_required(VERSION 3.15)
project(sig8 C)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
set(CMAKE_C_STANDARD 11)

find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)

add_executable(sig8
        src/main.c
        include/sig8.h
        src/config.h
        src/util.c
        src/render.c
        src/sig8_internal.h
        src/glad.h
        src/glad.c
        src/graphics.c
        src/resources/font5x7.c
        )

target_include_directories(sig8
PRIVATE
        "${CMAKE_SOURCE_DIR}/include/"
        "${CMAKE_SOURCE_DIR}/src/"
        SDL2
        OpenGL::GL
)

target_link_libraries(sig8
PRIVATE
        SDL2
        OpenGL::GL
        ${CMAKE_DL_LIBS}
)
