#include "sig8_internal.h"

int paletteMap[N_COLORS];
static Font currentFont;

// PICO-8 color scheme, licensed under CC0

const char *colorNames[N_COLORS] = {
        "#000000",
        "#1D2B53",
        "#7E2553",
        "#008751",
        "#AB5236",
        "#5F574F",
        "#C2C3C7",
        "#FFF1E8",
        "#FF004D",
        "#FFA300",
        "#FFEC27",
        "#00E436",
        "#29ADFF",
        "#83769C",
        "#FF77A8",
        "#FFCCAA"
};

void InitializeScreen(void)
{
    currentFont = font5x7;

    for (int i = 0; i < N_COLORS; ++i) {
        colorMap[i] = ColorFromHex(colorNames[i]);
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

void SetFont(Font font)
{
    currentFont = font;
}

void RemapColor(int oldColor, int newColor)
{
    paletteMap[oldColor] = newColor;
}

void ResetColors(void)
{
    for (int i = 0; i < N_COLORS; ++i) {
        paletteMap[i] = i;
    }
}

void DrawPixel(int x, int y, int color) {
    if (color == TRANSPARENT) {
        return;
    }
    color = paletteMap[color];
    if (color != TRANSPARENT && x >= 0 && y >= 0 && x < SCREEN_WIDTH && y < SCREEN_HEIGHT) {
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
