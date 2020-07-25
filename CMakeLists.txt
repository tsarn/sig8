cmake_minimum_required(VERSION 3.13)
set(SIG8_VERSION 0.1.0)
project(sig8 VERSION ${SIG8_VERSION} DESCRIPTION "A library for creating old-school games")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")

if( ${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
    set(CMAKE_EXECUTABLE_SUFFIX .html)
endif()

add_subdirectory(src)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    # Only compile examples and sig8tk if building the library itself
    add_subdirectory(examples)
    add_subdirectory(sig8tk)
endif()
