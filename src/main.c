#include "sig8_internal.h"

int main(int argc, char **argv)
{
    Initialize();

    int bg = DARK_BLUE;
    int fg = PEACH;

    while (!ShouldQuit()) {
        if (KeyJustPressed("Space")) {
            bg = rand() % 16;
            fg = rand() % 16;
        }

        ClearScreen(bg);
        DrawString(0, 0, fg, "Test!");
    }

    Finalize();
    return 0;
}
