#ifdef SIG8_COMPILE_EDITORS
#include "editors.h"

#define TOOLBAR_SIZE 10

static sig8_ManagedResource *editing;
static SpriteSheet spriteSheet;

void sig8_TileEditorInit(sig8_ManagedResource *what)
{
    editing = what;
    spriteSheet = GetCurrentSpriteSheet();
}

static void DrawStatusBar(void)
{
    FillRect(0, 0, SCREEN_WIDTH, TOOLBAR_SIZE, DARK_BLUE);
    FillRect(0, SCREEN_HEIGHT - TOOLBAR_SIZE, SCREEN_WIDTH, TOOLBAR_SIZE, DARK_BLUE);
}

void sig8_TileEditorTick(void)
{
    UseSpriteSheet(sig8_EDITORS_SPRITESHEET);
    SetCursorShape(CURSOR_ARROW);
    ClearScreen(INDIGO);
    DrawStatusBar();
}

#endif
