#include "sig8_internal.h"

int main(int argc, char **argv)
{
    Initialize();

    int bg = BLACK;
    int fg = WHITE;

    while (!ShouldQuit()) {
        MousePosition pos = GetMousePosition();

        if (MouseJustPressed(MOUSE_LEFT)) {
            fg = rand() % 16;
            bg = rand() % 16;
        }

        ClearScreen(bg);
        DrawString(0, 0, fg, Format("Mouse: (%d, %d)", pos.x, pos.y));
        DrawSprite(5, 20, icon);
    }

    Finalize();
    return 0;
}
