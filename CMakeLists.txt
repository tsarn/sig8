cmake_minimum_required(VERSION 3.13)
project(sig8 VERSION 0.1.0 DESCRIPTION "A library for creating old-school games")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

if( ${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
    set(CMAKE_EXECUTABLE_SUFFIX .html)
endif()

add_subdirectory(src)
add_subdirectory(examples)
add_subdirectory(sig8tk)
