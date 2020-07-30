#include "sig8.h"

SpriteSheet spriteSheet;
TileMap tileMap;

void mainLoop(void)
{
    static int offsetX = 0;
    static int offsetY = 0;

    ClearScreen(DARK_BLUE);

    DrawTileMap(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, offsetX, offsetY);

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
