set(EXAMPLE_NAME example-lines)
add_executable(${EXAMPLE_NAME} main.c)
target_link_libraries(${EXAMPLE_NAME} m)
sig8_platform(${EXAMPLE_NAME})
