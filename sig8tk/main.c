#include "sig8tk.h"

int main(int argc, char **argv)
{
    Initialize("sig8tk");

    SetFont(FONT_3X5);

    while (!ShouldQuit()) {
        ClearScreen(DARK_BLUE);
        ResetLayout();
    }

    Finalize();
    return 0;
}
