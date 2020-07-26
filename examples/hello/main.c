/*
 * Inspired by PICO-8's demo at https://www.lexaloffle.com/pico-8.php?page=faq
 */

#include "sig8.h"
#include <math.h>

const char *message = "Have fun with SIG-8!";
Instrument instrument;
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
            DrawSprite(13 + i * 8, y, 10 + i,SPRITE_MASK_COLOR(BLACK));
        }

        if (KeyJustPressed("Z")) {
            PlayNote(0, A4);
        }

        if (KeyJustReleased("Z")) {
            StopNote(0);
        }

        if (KeyJustPressed("E")) {
            EditResource(spriteSheet);
        }
    }

    DrawString(23, 80, PEACH, message);
    DrawSprite(60, 90, 0, 0);

    ++t;
}

int main()
{
    Initialize("sig8 example: hello");
    instrument = NewInstrument();
    SetInstrument(0, instrument);
    spriteSheet = LoadSpriteSheet("res://spritesheet.png");
    UseSpriteSheet(spriteSheet);
    RunMainLoop(mainLoop);
    return 0;
}
