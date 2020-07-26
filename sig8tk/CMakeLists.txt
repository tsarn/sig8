add_executable(sig8tk
        main.c
        sig8tk.h
        sprites.c
        ui.c
        history.c)

if(MSVC)
    target_compile_options(sig8tk PRIVATE /W4)
else()
    target_compile_options(sig8tk PRIVATE -Wall -Wextra -pedantic)
endif()

sig8_bundle(
        TARGET sig8tk
        RESOURCES spritesheet.png
)

sig8_platform(sig8tk)
