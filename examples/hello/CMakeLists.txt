set(EXAMPLE_NAME hello)
add_executable(example-${EXAMPLE_NAME}
        main.c
        )
target_compile_features(example-${EXAMPLE_NAME} PRIVATE c_std_99)
target_link_libraries(example-${EXAMPLE_NAME} sig8 m)

if( ${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
    target_link_options(example-${EXAMPLE_NAME} PRIVATE
            --shell-file "${PROJECT_SOURCE_DIR}/platform/emscripten/shell_minimal.html"
            --preload-file "${CMAKE_CURRENT_SOURCE_DIR}/spritesheet.png@spritesheet.png"
    )
endif()
