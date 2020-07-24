#include "sig8tk.h"

int main()
{
    InitializeEx((Configuration){
        .windowName = "sig8 toolkit",
        .width = 200,
        .height = 146,
        .palette = PALETTE_DEFAULT,
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
