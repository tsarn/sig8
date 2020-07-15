#include "sig8_internal.h"

int paletteMap[N_COLORS];
static Font currentFont;
Rect area;
Rect areaStack[MAX_LAYOUT_NESTING];
int areaStackSize;

// PICO-8 color scheme, licensed under CC0

const char *colorNames[N_COLORS] = {
        "#000000",
        "#1D2B53",
        "#7E2553",
        "#008751",
        "#AB5236",
        "#5F574F",
        "#C2C3C7",
        "#FF004D",
        "#FFA300",
        "#FFEC27",
        "#00E436",
        "#29ADFF",
        "#83769C",
        "#FF77A8",
        "#FFCCAA",
        "#FFF1E8",
};

void InitializeScreen(void)
{
    currentFont = FONT_5X7;

    for (int i = 0; i < N_COLORS; ++i) {
        colorMap[i] = ColorFromHex(colorNames[i]);
        paletteMap[i] = i;
    }

    ClearScreen(0);
    ResetAreaStack();
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

void SetArea(int x, int y, int w, int h)
{
    area.x += x;
    area.y += y;
    area.width = w;
    area.height = h;
}

void ResetArea(void)
{
    area.x = area.y = 0;
    area.width = SCREEN_WIDTH;
    area.height = SCREEN_HEIGHT;
}

void ResetAreaStack(void)
{
    areaStackSize = 0;
    ResetArea();
}

void PushArea(void)
{
    areaStack[areaStackSize++] = area;
}

void PopArea(void)
{
    if (areaStackSize > 0) {
        area = areaStack[--areaStackSize];
    } else {
        ResetAreaStack();
    }
}

Rect GetArea(void)
{
    return area;
}

int GetAreaWidth(void)
{
    return area.width;
}

int GetAreaHeight(void)
{
    return area.height;
}

void DrawPixel(int x, int y, int color) {
    if (color == TRANSPARENT) {
        return;
    }

    x += area.x;
    y += area.y;
    color = paletteMap[color];

    if (color != TRANSPARENT && x >= area.x && y >= area.y &&
        x < area.x + area.width && y < area.y + area.height) {
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
        uint8_t line = currentFont->data[uch * currentFont->width + i];
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
            DrawPixel(x + i, y + j, sprite->data[(sy + j) * w + sx + i]);
        }
    }
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

void DrawHLine(int x, int y, int w, int color)
{
    for (int i = 0; i < w; ++i) {
        DrawPixel(x + i, y, color);
    }
}

void DrawVLine(int x, int y, int h, int color)
{
    for (int i = 0; i < h; ++i) {
        DrawPixel(x, y + i, color);
    }
}
