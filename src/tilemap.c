#include "sig8_internal.h"

static TileMap currentTileMap;

TileMap NewTileMap(void)
{
    return calloc(TILEMAP_WIDTH * TILEMAP_HEIGHT, sizeof(*currentTileMap));
}

void FreeTileMap(TileMap tileMap)
{
    free(tileMap);
}

void UseTileMap(TileMap tileMap)
{
    currentTileMap = tileMap;
}

void SetTile(int x, int y, int tile)
{
    currentTileMap[x + y * TILEMAP_WIDTH] = tile;
}

int GetTile(int x, int y)
{
    if (x < 0 || y < 0 || x >= TILEMAP_WIDTH || y >= TILEMAP_HEIGHT) {
        return -1;
    }

    return currentTileMap[x + y * TILEMAP_WIDTH];
}

static void DrawTileMapDefaultCallback(int x, int y, int *sprite, int *flags)
{
}

void DrawTileMap(int x, int y, int width, int height, int offsetX, int offsetY)
{
    DrawTileMapEx(x, y, width, height, offsetX, offsetY, DrawTileMapDefaultCallback);
}

void DrawTileMapEx(int x, int y, int width, int height, int offsetX, int offsetY, TileMapDrawCallback callback)
{
    for (int i = Divide(offsetX, SPRITE_WIDTH); SPRITE_WIDTH * i < width + offsetX; ++i) {
        for (int j = Divide(offsetY, SPRITE_HEIGHT); SPRITE_HEIGHT * j < height + offsetY; ++j) {
            int sprite = GetTile(i, j);
            if (sprite == -1) {
                continue;
            }

            int x0 = SPRITE_WIDTH * i - offsetX;
            int y0 = SPRITE_HEIGHT * j - offsetY;
            int x1 = x0;
            int y1 = y0;
            int x2 = SPRITE_WIDTH * (i + 1) - offsetX;
            int y2 = SPRITE_HEIGHT * (j + 1) - offsetY;

            if (x1 < 0) {
                x1 = 0;
            }

            if (y1 < 0) {
                y1 = 0;
            }

            if (x2 >= width) {
                x2 = width;
            }

            if (y2 >= height) {
                y2 = height;
            }

            int flags = 0;
            callback(i, j, &sprite, &flags);

            DrawSubSprite(x + x1, y + y1, sprite, flags, x1 - x0, y1 - y0, x2 - x0, y2 - y0);
        }
    }
}
