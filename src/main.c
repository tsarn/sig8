#include "sig8_internal.h"

int main(int argc, char **argv)
{
    Initialize();

    int bg = DARK_BLUE;
    int fg = PEACH;

    SetFont(&fontMono3x5);

    while (!ShouldQuit()) {
        if (KeyJustPressed("Space")) {
            bg = rand() % 16;
            fg = rand() % 16;
        }

        ClearScreen(bg);
        DrawString(50, 5, fg, "**** SIG-8 ****");
    }

    Finalize();
    return 0;
}
