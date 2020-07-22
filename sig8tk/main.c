#include "sig8tk.h"

int main()
{
    Initialize("sig8tk");
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
