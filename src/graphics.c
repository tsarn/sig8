#include "sig8_internal.h"

static Font currentFont;

int GetScreenWidth(void)
{
    return SCREEN_WIDTH;
}

int GetScreenHeight(void)
{
    return SCREEN_HEIGHT;
}

void InitializeScreen(void)
{
    currentFont = font5x7;

    for (int i = 0; i < N_COLORS; ++i) {
        colorMap[i] = ColorFromHex(ColorNames[i]);
    }

    ClearScreen(0);
}

void ClearScreen(int color)
{
    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; ++i) {
        screenBuffer[i] = colorMap[color];
    }
}

void SetFont(Font font)
{
    currentFont = font;
}

void DrawPixel(int x, int y, int color) {
    if (x >= 0 && y >= 0 && x < SCREEN_WIDTH && y < SCREEN_HEIGHT && color != TRANSPARENT) {
        screenBuffer[x + SCREEN_WIDTH * y] = colorMap[color];
    }
}

void DrawCharacter(int x, int y, int color, char ch)
{
    uint8_t uch = (uint8_t)ch;
    if (uch < currentFont->firstCharCode || uch > currentFont->lastCharCode) {
        return;
    }

    uch -= currentFont->firstCharCode;
    for (int i = 0; i < currentFont->width; ++i) {
        uint8_t line = currentFont->fontData[uch * currentFont->width + i];
        for (int j = 0; j < currentFont->height; ++j) {
            if ((line >> j) & 1) {
                DrawPixel(x + i, y + j, color);
            }
        }
    }
}

void DrawString(int x, int y, int color, const char *string)
{
    for (const char *c = string; *c; ++c) {
        DrawCharacter(x, y, color, *c);

        x += currentFont->horizontalStep + currentFont->width;
    }
}

void DrawSprite(int x, int y, Sprite sprite)
{
    DrawSubSprite(x, y, sprite, 0, 0, sprite->width, sprite->height);
}

void DrawSubSprite(int x, int y, Sprite sprite, int sx, int sy, int w, int h)
{
    for (int j = 0; j < h; ++j) {
        for (int i = 0; i < w; ++i) {
            DrawPixel(x + i, y + j, sprite->spriteData[(sy + j) * w + sx + i]);
        }
    }
}
