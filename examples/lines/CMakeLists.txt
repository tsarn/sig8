set(EXAMPLE_NAME lines)
add_executable(example-${EXAMPLE_NAME}
        main.c)
target_compile_features(example-${EXAMPLE_NAME} PRIVATE c_std_11)
target_link_libraries(example-${EXAMPLE_NAME} sig8 m)
