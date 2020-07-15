#include "sig8_internal.h"

int main(int argc, char **argv)
{
    Initialize();

    int bg = DARK_BLUE;
    int fg = WHITE;

    while (!ShouldQuit()) {
        MousePosition pos = GetMousePosition();

        ClearScreen(bg);
        DrawString(0, 0, fg, Format("Mouse: (%d, %d)", pos.x, pos.y));
        DrawSprite(5, 0, icon);
    }

    Finalize();
    return 0;
}
