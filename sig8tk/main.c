#include "sig8tk.h"

int main(int argc, char **argv)
{
    Initialize("sig8tk");

    SetFont(FONT_3X5);

    InitSpriteEditor();

    while (!ShouldQuit()) {
        DrawSpriteEditor();
    }

    Finalize();
    return 0;
}
