#include <stdlib.h>
#include "sig8.h"

SpriteSheet spriteSheet;

void mainLoop(void)
{
    static int offsetX = 0;
    static int offsetY = 0;

    ClearScreen(BLACK);

    DrawTileMap(16, 16, 96, 96, offsetX, offsetY);

    if (KeyJustPressed("Right")) offsetX += 5;
    if (KeyJustPressed("Up")) offsetY -= 5;
    if (KeyJustPressed("Down")) offsetY += 5;
    if (KeyJustPressed("Left")) offsetX -= 5;
    if (KeyJustPressed("E")) {
        EditResource(spriteSheet);
    }
}

int main()
{
    Initialize("sig8 example: tiles");

    UseTileMap(NewTileMap());
    spriteSheet = LoadSpriteSheet("res://spritesheet.png");
    UseSpriteSheet(spriteSheet);

    for (int i = 0; i < TILEMAP_WIDTH; ++i) {
        for (int j = 0; j < TILEMAP_HEIGHT; ++j) {
            SetTile(i, j, rand() % 16);
        }
    }

    RunMainLoop(mainLoop);
    return 0;
}
