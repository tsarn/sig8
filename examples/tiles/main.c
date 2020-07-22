#include <stdlib.h>
#include "sig8.h"

int main()
{
    Initialize("sig8 example: tiles");

    UseTileMap(NewTileMap());
    UseSpriteSheet(SpriteSheetFromImage("spritesheet.png"));

    for (int i = 0; i < TILEMAP_WIDTH; ++i) {
        for (int j = 0; j < TILEMAP_HEIGHT; ++j) {
            SetTile(i, j, rand() % 16);
        }
    }

    int offsetX = 0;
    int offsetY = 0;

    while (Tick()) {
        ClearScreen(BLACK);

        DrawTileMap(16, 16, 96, 96, offsetX, offsetY);

        if (KeyJustPressed("Right")) offsetX += 5;
        if (KeyJustPressed("Up")) offsetY -= 5;
        if (KeyJustPressed("Down")) offsetY += 5;
        if (KeyJustPressed("Left")) offsetX -= 5;
    }

    Finalize();
    return 0;
}
