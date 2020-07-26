#include "sig8_internal.h"
#include "stb_image.h"

static int *paletteMap;
static Font currentFont;
static SpriteSheet currentSpriteSheet;

static Color *screenBuffer;
static Color *colorMap;

#ifdef SIG8_COMPILE_EDITORS
static Color *savedScreenBuffer;
static Font savedFont;
static SpriteSheet savedSpriteSheet;
#endif

Palette PALETTE_DEFAULT = {
        .size = 16,
        .colors = (const char *[]){
            "#000000",
            "#eb2167",
            "#eb7a23",
            "#ebe723",
            "#26eb50",
            "#28bdeb",
            "#f57aab",
            "#f2c78f",
            "#722ca3",
            "#961743",
            "#734521",
            "#208061",
            "#194b63",
            "#465663",
            "#999999",
            "#f2f7f7",
        }
};

#ifdef SIG8_COMPILE_EDITORS
static void OnEditorEnter(void)
{
    ResetColors();
    savedScreenBuffer = malloc(sizeof(Color) * SCREEN_WIDTH * SCREEN_HEIGHT);
    memcpy(savedScreenBuffer, screenBuffer, sizeof(Color) * SCREEN_WIDTH * SCREEN_HEIGHT);
    savedFont = currentFont;
    savedSpriteSheet = currentSpriteSheet;
}

static void OnEditorLeave(void)
{
    ResetColors();
    memcpy(screenBuffer, savedScreenBuffer, sizeof(Color) * SCREEN_WIDTH * SCREEN_HEIGHT);
    free(savedScreenBuffer);
    currentFont = savedFont;
    currentSpriteSheet = savedSpriteSheet;
}
#endif

void sig8_InitScreen(Color *screen)
{
    Palette palette = GetPalette();

    if (!colorMap) {
        colorMap = malloc(sizeof(Color) * palette.size);
        paletteMap = malloc(sizeof(int) * palette.size);
#ifdef SIG8_COMPILE_EDITORS
        sig8_RegisterCallback(EDITOR_ENTER_EVENT, OnEditorEnter);
        sig8_RegisterCallback(EDITOR_LEAVE_EVENT, OnEditorLeave);
#endif
    }

    screenBuffer = screen;
    currentFont = FONT_ASEPRITE;

    for (int i = 0; i < palette.size; ++i) {
        colorMap[i] = ColorFromHex(palette.colors[i]);
        paletteMap[i] = i;
    }

    ClearScreen(0);
}

void ClearScreen(int color)
{
    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; ++i) {
        screenBuffer[i] = colorMap[color];
    }
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
        screenBuffer[x + SCREEN_WIDTH * y] = colorMap[color];
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
    x += sprite % 16 * SPRITE_WIDTH;
    y += sprite / 16 * SPRITE_HEIGHT;
    int idx = x + y * 16 * SPRITE_WIDTH;
    if (idx < 0 || idx >= SPRITE_SHEET_BYTE_SIZE) {
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

void DrawSprite(int x, int y, int sprite, int flags)
{
    DrawSubSprite(x, y, sprite, flags, 0, 0, SPRITE_WIDTH, SPRITE_HEIGHT);
}

void DrawSubSprite(int x, int y, int sprite, int flags, int sx, int sy, int w, int h)
{
    if (sprite < 0 || sprite >= SPRITE_SHEET_SIZE || !currentSpriteSheet) {
        return;
    }

    for (int i = 0; i < w; ++i) {
        for (int j = 0; j < h; ++j) {
            // coordinates inside of sprite
            int tx = i + sx;
            int ty = j + sy;

            // coordinates on screen relative to (x, y)
            int px = i;
            int py = j;

            if (flags & SPRITE_HFLIP) {
                px = w - 1 - i;
            }

            if (flags & SPRITE_VFLIP) {
                py = h - 1 - i;
            }

            if (flags & SPRITE_ROTATE_CW) {
                int t = px;
                px = py;
                py = w - 1 - t;
            }

            int color = GetSpritePixel(tx, ty, sprite);

            if (flags & SPRITE_ENABLE_MASK) {
                if (color == (flags >> 4)) {
                    continue;
                }
            }

            DrawPixel(px + x, py + y, color);
        }
    }
}

SpriteSheet LoadSpriteSheet(const char *path)
{
    uint8_t *result = sig8_AllocateResource(RESOURCE_SPRITESHEET, path, SPRITE_SHEET_BYTE_SIZE);

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

                    int bestColor = 0;
                    int err = INT32_MAX;
                    for (int color = 0; color < PALETTE_SIZE; ++color) {
                        int dr = colorMap[color].r - r;
                        int dg = colorMap[color].g - g;
                        int db = colorMap[color].b - b;
                        int curErr = dr * dr + dg * dg + db * db;
                        if (curErr < err) {
                            bestColor = color;
                            err = curErr;
                        }
                    }

                    result[GetSpritePixelIndex(x, y, sprite)] = bestColor;
                }
            }

            ++sprite;
        }
    }

    stbi_image_free(data);
    return result;
}
