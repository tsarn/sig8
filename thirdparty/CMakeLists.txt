add_subdirectory(stb_image)
add_subdirectory(stb_image_write)

if (NOT (${CMAKE_SYSTEM_NAME} MATCHES "Emscripten"))
    add_subdirectory(glad_gles3)
endif()
