#include "sig8tk.h"

int main(int argc, char **argv)
{
    Initialize("sig8tk");

    SetFont(FONT_3X5);

    while (!ShouldQuit()) {
        ClearScreen(BACKGROUND_COLOR);
        ResetArea();

        DrawingArea(5, 5, SCREEN_WIDTH - 10, SCREEN_HEIGHT - 10);
        FillRect(0, 0, GetAreaWidth(), 7, TOOLBAR_COLOR);
    }

    Finalize();
    return 0;
}
