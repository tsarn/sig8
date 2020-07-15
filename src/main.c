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
    }

    Finalize();
    return 0;
}
