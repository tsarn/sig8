#include "sig8tk.h"

int main(int argc, char **argv)
{
    Initialize("sig8tk");

    SetFont(FONT_3X5);

    while (!ShouldQuit()) {
        ClearScreen(BACKGROUND_COLOR);
        FillRect(0, 0, SCREEN_WIDTH, 7, TOOLBAR_COLOR);
    }

    Finalize();
    return 0;
}
