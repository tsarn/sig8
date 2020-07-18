#include "sig8.h"
#include <math.h>

int main()
{
    Initialize("sig8 example: hello");

    UseSpriteSheet(SpriteSheetFromImage("spritesheet.png"));

    int t = 0;

    SetFont(FONT_5X7);

    while (!ShouldQuit()) {
        ClearScreen(BLACK);

        for (int i = 0; i < 12; ++i) {
            for (int j = 0; j < 8; ++j) {
                int t1 = t + i * 4 - j * 4;
                int y = 45 - j + cosf(t1 / 10.0f) * 10.0f;
                RemapColor(WHITE, j == 7 ? WHITE : j);
                DrawSprite(36 + i * 8, y, 10 + i,SPRITE_MASK_COLOR(BLACK));
            }
        }

        DrawString(28, 75, PEACH, "Have fun with SIG-8!");
        DrawSprite(84, 90, 0, 0);

        ++t;
    }

    Finalize();
    return 0;
}
