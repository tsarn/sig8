#include "sig8.h"

SpriteSheet spriteSheet;
TileMap tileMap;

Palette PALETTE_DB32 = {
        .size = 32,
        .colors = (const char *[]){
                "#000000",
                "#222034",
                "#45283c",
                "#663931",
                "#8f563b",
                "#df7126",
                "#d9a066",
                "#eec39a",
                "#fbf236",
                "#99e550",
                "#6abe30",
                "#37946e",
                "#4b692f",
                "#524b24",
                "#323c39",
                "#3f3f74",
                "#306082",
                "#5b6ee1",
                "#639bff",
                "#5fcde4",
                "#cbdbfc",
                "#ffffff",
                "#9badb7",
                "#847e87",
                "#696a6a",
                "#595652",
                "#76428a",
                "#ac3232",
                "#d95763",
                "#d77bba",
                "#8f974a",
                "#8a6f30",
        }
};

void mainLoop(void)
{
    static int offsetX = 0;
    static int offsetY = 0;

    ClearScreen(20);

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
    UsePalette(PALETTE_DB32);

    tileMap = LoadTileMap("res://tilemap.dat");
    UseTileMap(tileMap);
    spriteSheet = LoadSpriteSheet("res://spritesheet.png");
    UseSpriteSheet(spriteSheet);

    RunMainLoop(mainLoop);
    return 0;
}
