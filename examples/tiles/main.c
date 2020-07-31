#include "sig8.h"

SpriteSheet spriteSheet;
TileMap tileMap;
int t;

void tileCallback(int x, int y, int *sprite, int *mask)
{
    if (*sprite >= 16) {
        *sprite += 2 * (t / 20 % 3);
    }
}

void mainLoop(void)
{
    static int offsetX = 0;
    static int offsetY = 0;

    ClearScreen(BLUE);

    DrawTileMapEx(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, offsetX, offsetY, 0, tileCallback);

    if (KeyJustPressed("Right")) offsetX += 5;
    if (KeyJustPressed("Up")) offsetY -= 5;
    if (KeyJustPressed("Down")) offsetY += 5;
    if (KeyJustPressed("Left")) offsetX -= 5;

    if (KeyJustPressed("Ctrl+E")) {
        EditResource(spriteSheet);
    }

    if (KeyJustPressed("E")) {
        EditResource(tileMap);
    }

    ++t;
}

int main()
{
    Initialize("sig8 example: tiles");

    tileMap = LoadTileMap("res://tilemap.dat");
    UseTileMap(tileMap);
    spriteSheet = LoadSpriteSheet("res://spritesheet.png");
    UseSpriteSheet(spriteSheet);

    RunMainLoop(mainLoop);
    return 0;
}
