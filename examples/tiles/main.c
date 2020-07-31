#include "sig8.h"

int t;

void tileCallback(int x, int y, int *sprite, int *mask)
{
    if (*sprite >= 16) {
        *sprite += 2 * (t / 20 % 3);
    }
}

void mainLoop(void)
{
    ClearScreen(BLUE);

    DrawTileMapEx(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, 0, tileCallback);

    if (KeyJustPressed("Ctrl+E")) {
        EditResource(GetCurrentSpriteSheet());
    }

    if (KeyJustPressed("E")) {
        EditResource(GetCurrentTileMap());
    }

    ++t;
}

int main()
{
    Initialize("sig8 example: tiles");
    UseTileMap(LoadTileMap("res://tilemap.dat"));
    UseSpriteSheet(LoadSpriteSheet("res://spritesheet.png"));
    RunMainLoop(mainLoop);
    return 0;
}
