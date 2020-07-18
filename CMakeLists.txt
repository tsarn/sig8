cmake_minimum_required(VERSION 3.15)
project(sig8 VERSION 0.1.0 DESCRIPTION "A library for creating old-school games")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)

add_subdirectory(src)
add_subdirectory(examples)
#add_subdirectory(sig8tk)
