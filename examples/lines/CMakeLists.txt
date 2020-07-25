set(EXAMPLE_NAME lines)
add_executable(example-${EXAMPLE_NAME}
        main.c)
target_compile_features(example-${EXAMPLE_NAME} PRIVATE c_std_11)
target_link_libraries(example-${EXAMPLE_NAME} sig8 m)

if( ${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
    target_link_options(example-${EXAMPLE_NAME} PRIVATE
            --shell-file "${PROJECT_SOURCE_DIR}/platform/emscripten/shell_minimal.html"
    )
endif()
