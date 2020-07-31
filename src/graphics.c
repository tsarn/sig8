#include "sig8_internal.h"
#include "stb_image.h"

static Font currentFont;
static SpriteSheet currentSpriteSheet;

static uint8_t *colorBuffer;
static Color *screenBuffer;
static Palette palette;

static int paletteMap[MAX_PALETTE_SIZE];
static Color colorMap[MAX_PALETTE_SIZE];

#ifdef SIG8_COMPILE_EDITORS
static Font savedFont;
static SpriteSheet savedSpriteSheet;
static TileMap savedTileMap;
#endif

Palette PALETTE_DEFAULT = {
        .size = 16,
        .colors = (Color[]){
            { 9, 8, 20 },
            { 222, 46, 107 },
            { 222, 124, 48 },
            { 222, 219, 48 },
            { 51, 222, 87 },
            { 53, 182, 222 },
            { 237, 130, 173 },
            { 236, 198, 149 },
            { 114, 44, 163 },
            { 150, 23, 67 },
            { 115, 69, 33 },
            { 32, 128, 97 },
            { 25, 75, 99 },
            { 70, 86, 99 },
            { 153, 153, 153 },
            { 242, 247, 247 },
        }
};

#ifdef SIG8_COMPILE_EDITORS
static void OnEditorEnter(void)
{
    ResetColors();
    savedFont = currentFont;
    savedSpriteSheet = currentSpriteSheet;
    savedTileMap = GetCurrentTileMap();
}

static void OnEditorLeave(void)
{
    ResetColors();
    currentFont = savedFont;
    currentSpriteSheet = savedSpriteSheet;
    UseTileMap(savedTileMap);
}
#endif

void sig8_InitScreen(Color *screen)
{
    if (colorBuffer) {
        free(colorBuffer);
    }

    colorBuffer = calloc(1, SCREEN_WIDTH * SCREEN_HEIGHT);

    static bool initialized = false;

    if (!initialized) {
#ifdef SIG8_COMPILE_EDITORS
        sig8_RegisterCallback(EDITOR_ENTER_EVENT, OnEditorEnter);
        sig8_RegisterCallback(EDITOR_LEAVE_EVENT, OnEditorLeave);
#endif
        UsePalette(PALETTE_DEFAULT);
        initialized = true;
    }

    screenBuffer = screen;
    currentFont = FONT_SMALL;

    ClearScreen(0);
}

void sig8_UpdateScreen(void)
{
    int size = SCREEN_WIDTH * SCREEN_HEIGHT;
    uint8_t *ptr = colorBuffer;
    for (int i = 0; i < size; ++i) {
        int color = *ptr++;
        if (color < MAX_PALETTE_SIZE) {
            screenBuffer[i] = colorMap[color];
        }
    }
}

void UsePalette(Palette newPalette)
{
    if (screenBuffer) {
        sig8_UpdateScreen();
        ClearScreen(0xff);
    }

    palette = newPalette;

    for (int i = 0; i < palette.size; ++i) {
        colorMap[i] = palette.colors[i];
        paletteMap[i] = i;
    }

}

Palette GetPalette(void)
{
    return palette;
}

void ClearScreen(int color)
{
    int size = SCREEN_WIDTH * SCREEN_HEIGHT;
    memset(colorBuffer, color, size);
}

void UseFont(Font font)
{
    currentFont = font;
}

void RemapColor(int oldColor, int newColor)
{
    paletteMap[oldColor] = newColor;
}

void ResetColors(void)
{
    for (int i = 0; i < PALETTE_SIZE; ++i) {
        paletteMap[i] = i;
    }
}

Color ColorFromIndex(int color)
{
    if (color < 0 || color >= PALETTE_SIZE) {
        return (Color){};
    }
    return colorMap[color];
}

void DrawPixel(int x, int y, int color) {
    if (color == TRANSPARENT) {
        return;
    }

    color = paletteMap[color];

    if (color != TRANSPARENT && x >= 0 && y >= 0 &&
        x < SCREEN_WIDTH && y < SCREEN_HEIGHT) {
        colorBuffer[x + SCREEN_WIDTH * y] = color;
    }
}

int GetPixel(int x, int y)
{
    if (x >= 0 && y >= 0 && x < SCREEN_WIDTH && y < SCREEN_HEIGHT) {
        return colorBuffer[x + SCREEN_WIDTH * y];
    } else {
        return 0;
    }
}

static int DrawCharacter(int x, int y, int color, char ch)
{
    y -= currentFont->height - currentFont->lineHeight;
    uint8_t uch = (uint8_t)ch;
    if (uch < currentFont->firstCharCode || uch > currentFont->lastCharCode) {
        return 0;
    }

    uch -= currentFont->firstCharCode;

    if (currentFont->isMono) {
        if (color != TRANSPARENT) {
            for (int i = 0; i < currentFont->width; ++i) {
                uint8_t line = currentFont->data[uch * currentFont->width + i];
                for (int j = 0; j < currentFont->height; ++j) {
                    if ((line >> j) & 1) {
                        DrawPixel(x + i, y + j, color);
                    }
                }
            }
        }
        return currentFont->width;
    } else {
        const uint8_t *data = &currentFont->data[(currentFont->width + 2) * uch];
        int width = data[0];
        int height = data[1];

        if (color != TRANSPARENT) {
            for (int i = 0; i < width; ++i) {
                uint8_t line = data[2 + i];
                for (int j = 0; j < height; ++j) {
                    if ((line >> j) & 1) {
                        DrawPixel(x + i, y + j, color);
                    }
                }
            }
        }

        return width;
    }
}

void DrawString(int x, int y, int color, const char *string)
{
    for (const char *c = string; *c; ++c) {
        x += currentFont->horizontalStep + DrawCharacter(x, y, color, *c);
    }
}

int MeasureString(const char *string) {
    int res = 0;

    for (const char *c = string; *c; ++c) {
        res += currentFont->horizontalStep + DrawCharacter(0, 0, TRANSPARENT, *c);
    }

    return res;
}

void StrokeRect(int x, int y, int w, int h, int color)
{
    for (int i = x; i < x + w; ++i) {
        DrawPixel(i, y, color);
        DrawPixel(i, y + h - 1, color);
    }

    for (int i = y; i < y + h; ++i) {
        DrawPixel(x, i, color);
        DrawPixel(x + w - 1, i, color);
    }
}

void FillRect(int x, int y, int w, int h, int color)
{
    for (int j = y; j < y + h; ++j) {
        for (int i = x; i < x + w; ++i) {
            DrawPixel(i, j, color);
        }
    }
}

static void DrawLineImpl(int x0, int y0, int x1, int y1, bool swap, int color)
{
    int dx = x1 - x0;
    int dy = y1 - y0;

    int delta = 1;
    if (dy < 0) {
        delta = -1;
        dy = -dy;
    }

    int err = 2 * dy - dx;
    int y = y0;

    for (int x = x0; x <= x1; ++x) {
        if (swap) {
            DrawPixel(x, y, color);
        } else {
            DrawPixel(y, x, color);
        }

        if (err > 0) {
            y += delta;
            err -= 2 * dx;
        }
        err += 2 * dy;
    }
}

void DrawLine(int x0, int y0, int x1, int y1, int color)
{
    if (abs(y1 - y0) < abs(x1 - x0)) {
        if (x0 > x1) {
            DrawLineImpl(x1, y1, x0, y0, true, color);
        } else {
            DrawLineImpl(x0, y0, x1, y1, true, color);
        }
    } else {
        if (y0 > y1) {
            DrawLineImpl(y1, x1, y0, x0, false, color);
        } else {
            DrawLineImpl(y0, x0, y1, x1, false, color);
        }
    }
}

void UseSpriteSheet(SpriteSheet spriteSheet)
{
    currentSpriteSheet = spriteSheet;
}

void FreeSpriteSheet(SpriteSheet spriteSheet)
{
    sig8_FreeResource(spriteSheet);
}

SpriteSheet GetCurrentSpriteSheet(void)
{
    return currentSpriteSheet;
}

static inline int GetSpritePixelIndex(int x, int y, int sprite)
{
    x += sprite % SPRITESHEET_WIDTH * SPRITE_WIDTH;
    y += sprite / SPRITESHEET_WIDTH * SPRITE_HEIGHT;
    int idx = x + y * SPRITESHEET_WIDTH * SPRITE_WIDTH;
    if (idx < 0 || idx >= SPRITESHEET_BYTE_SIZE) {
        return -1;
    }
    return idx;
}

int GetSpritePixel(int x, int y, int sprite)
{
    int idx = GetSpritePixelIndex(x, y, sprite);
    if (idx == -1) {
        return TRANSPARENT;
    }
    return currentSpriteSheet[idx];
}

void SetSpritePixel(int x, int y, int sprite, int color)
{
    int idx = GetSpritePixelIndex(x, y, sprite);
    if (idx != -1) {
        currentSpriteSheet[idx] = color;
    }
}

static inline void DrawSpriteImpl(int x, int y, int sx, int sy, int w, int h, int mask)
{
    int hSize = SCREEN_WIDTH - x;
    int vSize = SCREEN_HEIGHT - y;
    int hOff = (x < 0) ? -x : 0;
    x += hOff;
    int vOff = (y < 0) ? -y : 0;
    y += vOff;
    int width = SCREEN_WIDTH;

    if (hSize <= 0 || vSize <= 0) {
        return;
    }

    if (hSize > w) {
        hSize = w;
    }

    if (vSize > h) {
        vSize = h;
    }

    uint8_t *sprPtr = &currentSpriteSheet[
            sx +
            sy * SPRITESHEET_WIDTH * SPRITE_WIDTH +
            hOff + vOff * (SPRITESHEET_WIDTH * SPRITE_WIDTH)
    ];

    uint8_t *scrPtr = &colorBuffer[x + y * width];

    for (int j = vOff; j < vSize; ++j) {
        for (int i = hOff; i < hSize; ++i, ++sprPtr, ++scrPtr) {
            if (*sprPtr != mask) {
                *scrPtr = paletteMap[*sprPtr];
            }
        }

        sprPtr += SPRITESHEET_WIDTH * SPRITE_WIDTH - hSize + hOff;
        scrPtr += width - hSize + hOff;
    }
}

void DrawSprite(int x, int y, int sprite)
{
    DrawSubSprite(x, y, sprite, 0, 0, SPRITE_WIDTH, SPRITE_HEIGHT, 0);
}

void DrawSpriteMask(int x, int y, int sprite, int mask)
{
    DrawSubSprite(x, y, sprite, 0, 0, SPRITE_WIDTH, SPRITE_HEIGHT, mask);
}

void DrawBigSprite(int x, int y, int sprite, int w, int h)
{
    DrawSubSprite(x, y, sprite, 0, 0, w * SPRITE_WIDTH, h * SPRITE_HEIGHT, 0);
}

void DrawBigSpriteMask(int x, int y, int sprite, int w, int h, int mask)
{
    DrawSubSprite(x, y, sprite, 0, 0, w * SPRITE_WIDTH, h * SPRITE_HEIGHT, mask);
}

void DrawSubSprite(int x, int y, int sprite, int sx, int sy, int w, int h, int mask)
{
    if (sprite < 0 || sprite >= SPRITESHEET_SIZE || !currentSpriteSheet) {
        return;
    }

    sx += sprite % SPRITESHEET_WIDTH * SPRITE_WIDTH;
    sy += sprite / SPRITESHEET_WIDTH * SPRITE_HEIGHT;

    DrawSpriteImpl(x, y, sx, sy, w, h, mask);
}

int GetBestColor(int r, int g, int b)
{
    int bestColor = 0;
    int err = INT32_MAX;
    for (int i = 0; i < PALETTE_SIZE; ++i) {
        int dr = colorMap[i].r - r;
        int dg = colorMap[i].g - g;
        int db = colorMap[i].b - b;
        int curErr = dr * dr + dg * dg + db * db;
        if (curErr < err) {
            bestColor = i;
            err = curErr;
        }
    }
    return bestColor;
}

SpriteSheet LoadSpriteSheet(const char *path)
{
    uint8_t *result = sig8_AllocateResource(RESOURCE_SPRITESHEET, path, SPRITESHEET_BYTE_SIZE);

    int width, height, channels, fileSize;
    uint8_t *contents = ReadFileContents(path, &fileSize);

    if (!contents) {
        printf("WARNING: Failed to load sprite sheet '%s'\n", path);
        return result;
    }

    uint8_t *data = stbi_load_from_memory(contents, fileSize, &width, &height, &channels, 3);
    free(contents);

    if (!data) {
        printf("WARNING: Failed to load sprite sheet '%s'\n", path);
        return result;
    }

    int sizeX = width / SPRITE_WIDTH;
    int sizeY = height / SPRITE_HEIGHT;
    int sprite = 0;

    for (int i = 0; i < sizeY; ++i) {
        for (int j = 0; j < sizeX; ++j) {
            int imageX = j * SPRITE_WIDTH;
            int imageY = i * SPRITE_HEIGHT;

            for (int y = 0; y < SPRITE_HEIGHT; ++y) {
                for (int x = 0; x < SPRITE_WIDTH; ++x) {
                    uint8_t *pixel = &data[3 * (imageX + x + (imageY + y) * width)];

                    int r = pixel[0];
                    int g = pixel[1];
                    int b = pixel[2];

                    result[GetSpritePixelIndex(x, y, sprite)] = GetBestColor(r, g, b);
                }
            }

            ++sprite;
        }
    }

    stbi_image_free(data);
    return result;
}
