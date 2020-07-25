#include "sig8tk.h"

int main()
{
    InitializeEx((Configuration){
        .windowName = "sig8 toolkit",
        .width = 200,
        .height = 146,
        .palette = PALETTE_DEFAULT,
    });
    MAIN_SPRITESHEET = LoadSpriteSheet("spritesheet.png");

    SpritesInit();

    bool performNext = false;

    while (Tick()) {
        if (!AnyEventsHappened() && !performNext) continue;
        UseSpriteSheet(MAIN_SPRITESHEET);
        SetCursorShape(CURSOR_ARROW);
        SpritesTick();
        performNext = AnyEventsHappened();
    }

    Finalize();
    return 0;
}
