add_executable(sig8tk
        main.c
        sig8tk.h
        sprites.c
        ui.c
)

sig8_bundle(
        TARGET sig8tk
        RESOURCES spritesheet.png
)

sig8_platform(sig8tk)
