#include "sig8tk.h"

int main()
{
    InitializeEx((Configuration){
        .windowName = "sig8 toolkit",
        .width = 192,
        .height = 144
    });
    MAIN_SPRITESHEET = SpriteSheetFromImage("spritesheet.png");

    SpritesInit();

    while (Tick()) {
        UseSpriteSheet(MAIN_SPRITESHEET);
        SetCursorShape(CURSOR_ARROW);
        SpritesTick();
    }

    Finalize();
    return 0;
}
