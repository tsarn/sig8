set(EXAMPLE_NAME example-hello)
add_executable(${EXAMPLE_NAME} main.c)
target_link_libraries(${EXAMPLE_NAME} m)

sig8_bundle(
    ${EXAMPLE_NAME}
    RESOURCES spritesheet.png
)

sig8_platform(${EXAMPLE_NAME})
