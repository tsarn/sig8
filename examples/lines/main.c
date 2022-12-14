/*
 * Inspired by TIC-80's line demo at https://github.com/nesbox/TIC-80/wiki/line
 */

#include "sig8.h"
#include <math.h>

static void mainLoop(void)
{
    static float t = 0.0f;

    ClearScreen(BLACK);

    t += GetDelta();

    for (int i = 0; i < 128; i += 8) {
        int x = (int)roundf(fmodf(i + t * 16.0f, 128.0f));
        DrawLine(0, x, 128 - x, 0, BLUE);
        DrawLine(x, 127, 127, 128 - x, RED);
    }

    for (int i = 0; i < 64; i += 16) {
        int x = (int)roundf(fmodf(i + t * 16.0f, 64.0f)) + 32;
        DrawLine(127, 0, x, x, WHITE);
        DrawLine(0, 127, x, x, WHITE);
        DrawLine(127, 0, 128 - x, 128 - x, WHITE);
        DrawLine(0, 127, 128 - x, 128 - x, WHITE);
    }
}

int main()
{
    Initialize("sig8 example: lines");
    RunMainLoop(mainLoop);
    return 0;
}
