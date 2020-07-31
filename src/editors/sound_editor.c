#include "editors.h"

static Palette palette;

static void HandleInput(void)
{
    if (KeyJustPressed("Escape")) {
        SetCursorShape(CURSOR_ARROW);
        sig8_HistoryClear();
        sig8_LeaveEditor();
        UsePalette(palette);
        return;
    }
}

void sig8_SoundEditorInit(ManagedResource *what)
{
    sig8_Editing = what;
    palette = GetPalette();
    UsePalette(PALETTE_DEFAULT);
}

void sig8_SoundEditorTick(void)
{
    UseSpriteSheet(sig8_EDITORS_SPRITESHEET);
    SetCursorShape(CURSOR_ARROW);
    UseFont(FONT_MEDIUM);
    ClearScreen(BLACK);
    HandleInput();
}
