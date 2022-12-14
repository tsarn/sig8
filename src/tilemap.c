#include "sig8_internal.h"
#include "stb_image.h"

static TileMap currentTileMap;

TileMap NewTileMap(void)
{
    return sig8_AllocateResource(RESOURCE_TILEMAP, NULL, TILEMAP_BYTE_SIZE);
}

TileMap LoadTileMap(const char *path)
{
    uint8_t *result = sig8_AllocateResource(RESOURCE_TILEMAP, path, TILEMAP_BYTE_SIZE);

    int compressedSize;
    char *compressed = (char *)ReadFileContents(path, &compressedSize);

    if (!compressed) {
        printf("WARNING: Failed to load tile map '%s'\n", path);
        return result;
    }

    stbi_zlib_decode_buffer((char *)result, TILEMAP_BYTE_SIZE, compressed, compressedSize);
    free(compressed);

    return result;
}

void FreeTileMap(TileMap tileMap)
{
    sig8_FreeResource(tileMap);
}

void UseTileMap(TileMap tileMap)
{
    currentTileMap = tileMap;
}

TileMap GetCurrentTileMap(void)
{
    return currentTileMap;
}

void SetTile(int x, int y, int tile)
{
    currentTileMap[Modulo(x, TILEMAP_WIDTH) + Modulo(y, TILEMAP_HEIGHT) * TILEMAP_WIDTH] = tile;
}

int GetTile(int x, int y)
{
    return currentTileMap[Modulo(x, TILEMAP_WIDTH) + Modulo(y, TILEMAP_HEIGHT) * TILEMAP_WIDTH];
}

void DrawTileMap(int x, int y, int width, int height, int offsetX, int offsetY)
{
    DrawTileMapEx(x, y, width, height, offsetX, offsetY, 0, NULL);
}

void DrawTileMapEx(int x, int y, int width, int height, int offsetX, int offsetY, int mask, TileMapDrawCallback callback)
{
    for (int j = Divide(offsetY, SPRITE_HEIGHT); SPRITE_HEIGHT * j < height + offsetY; ++j) {
        for (int i = Divide(offsetX, SPRITE_WIDTH); SPRITE_WIDTH * i < width + offsetX; ++i) {
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

            int thisMask = mask;
            if (callback) {
                callback(i, j, &sprite, &thisMask);
            }

            DrawSubSprite(x + x1, y + y1, sprite, x1 - x0, y1 - y0, x2 - x1, y2 - y1, thisMask);
        }
    }
}
