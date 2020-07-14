#include "sig8_internal.h"

int main(int argc, char **argv)
{
    Initialize();

    while (!ShouldQuit()) {
        ClearScreen(DARK_BLUE);
        DrawString(5, 5, PEACH, Format("FPS: %.1f", 1.0f / GetDelta()));
    }

    Finalize();
    return 0;
}
