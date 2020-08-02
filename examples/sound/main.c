#include "sig8.h"

static void mainLoop(void)
{
    ClearScreen(BLACK);

    if (KeyJustPressed("E")) {
        EditResource(GetCurrentSoundLib());
    }

    if (KeyJustPressed("Space")) {
        PlaySound(0, 0);
    }

    if (KeyJustReleased("Space")) {
        StopNote(0);
    }
}

int main()
{
    Initialize("sig8 example: sound");
    UseSoundLib(LoadSoundLib("res://sounds.dat"));
    RunMainLoop(mainLoop);
    return 0;
}
