cmake_minimum_required(VERSION 3.13)
set(SIG8_VERSION 0.1.0)
project(sig8 VERSION ${SIG8_VERSION} DESCRIPTION "A library for creating old-school games")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")

if( ${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
    set(CMAKE_EXECUTABLE_SUFFIX .html)
endif()

option(SIG8_COMPILE_EDITORS "Compile library with editor support" ON)
option(SIG8_ENABLE_ASAN "Enable address sanitizer" OFF)

if( ${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
    option(SIG8_USE_GLAD "Use GLAD to load OpenGL ES" OFF)
else()
    option(SIG8_USE_GLAD "Use GLAD to load OpenGL ES" ON)
endif()

include(sig8util)
add_subdirectory(thirdparty)
add_subdirectory(src)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    # Only compile examples if building the library itself
    add_subdirectory(examples)
endif()
