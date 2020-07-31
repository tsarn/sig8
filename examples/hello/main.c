/*
 * Inspired by PICO-8's demo at https://www.lexaloffle.com/pico-8.php?page=faq
 */

#include "sig8.h"
#include <math.h>

const char *message = "Have fun with SIG-8!";
SpriteSheet spriteSheet;

void mainLoop(void)
{
    static int t = 0;
    ClearScreen(BLACK);

    for (int i = 0; i < 12; ++i) {
        for (int j = 0; j < 8; ++j) {
            int t1 = t + i * 4 - j * 4;
            int y = 45 - j + cosf(t1 / 10.0f) * 10.0f;
            RemapColor(WHITE, j == 7 ? WHITE : j);
            DrawSprite(13 + i * 8, y, 10 + i);
        }

        ResetColors();

        if (KeyJustPressed("E")) {
            EditResource(GetCurrentSpriteSheet());
        }
    }

    DrawString(24, 80, PEACH, message);
    DrawSprite(60, 90, 0);

    ++t;
}

int main()
{
    Initialize("sig8 example: hello");
    UseSpriteSheet(LoadSpriteSheet("res://spritesheet.png"));
    RunMainLoop(mainLoop);
    return 0;
}
