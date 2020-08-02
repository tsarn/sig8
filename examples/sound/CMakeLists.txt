set(EXAMPLE_NAME example-sound)
add_executable(${EXAMPLE_NAME} main.c)
sig8_platform(${EXAMPLE_NAME})
sig8_bundle(${EXAMPLE_NAME} RESOURCES sounds.dat)
