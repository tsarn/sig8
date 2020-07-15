#include "sig8_internal.h"

static const Font *currentFont;

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
    currentFont = &fontMono5x7;

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

void SetFont(const Font *font)
{
    currentFont = font;
}

void DrawPixel(int x, int y, int color) {
    screenBuffer[x + SCREEN_WIDTH * y] = colorMap[color];
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
