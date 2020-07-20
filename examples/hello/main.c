/*
 * Inspired by PICO-8's demo at https://www.lexaloffle.com/pico-8.php?page=faq
 */

#include "sig8.h"
#include <math.h>
#include <stdlib.h>

const char *message = "Have fun with SIG-8!";

int main()
{
    Initialize("sig8 example: hello");

    UseSpriteSheet(SpriteSheetFromImage("spritesheet.png"));

    Instrument testInstrument;
    testInstrument.volume = 1.0f;
    testInstrument.wave = TRIANGLE_WAVE;
    SetInstrument(0, testInstrument);
    PlayNote(0, A4);

    int t = 0;

    while (Tick()) {
        ClearScreen(BLACK);

        for (int i = 0; i < 12; ++i) {
            for (int j = 0; j < 8; ++j) {
                int t1 = t + i * 4 - j * 4;
                int y = 45 - j + cosf(t1 / 10.0f) * 10.0f;
                RemapColor(WHITE, j == 7 ? WHITE : j);
                DrawSprite(36 + i * 8, y, 10 + i,SPRITE_MASK_COLOR(BLACK));
            }
        }

        if (KeyJustPressed("Space")) {
            PlayNote(0, rand() % 20 + 40);
        }

        DrawString((SCREEN_WIDTH - MeasureString(message)) / 2, 80, PEACH, message);
        DrawSprite(84, 90, 0, 0);

        ++t;
    }

    Finalize();
    return 0;
}
