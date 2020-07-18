#include "sig8_internal.h"

int paletteMap[N_COLORS];
Font currentFont;
SpriteSheet currentSpriteSheet;

// PICO-8 color palette, licensed under CC0
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

    if (color != TRANSPARENT && x >= 0 && y >= 0 &&
        x < SCREEN_WIDTH && y < SCREEN_HEIGHT) {
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
            DrawLineImpl(x1, y1, x0, y0, false, color);
        } else {
            DrawLineImpl(x0, y0, x1, y1, false, color);
        }
    } else {
        if (y0 > y1) {
            DrawLineImpl(y1, x1, y0, x0, true, color);
        } else {
            DrawLineImpl(y0, x0, y1, x1, true, color);
        }
    }
}

void UseSpriteSheet(SpriteSheet spriteSheet)
{
    currentSpriteSheet = spriteSheet;
}

void DrawSprite(int x, int y, int sprite, int flags)
{
    DrawSubSprite(x, y, sprite, flags, 0, 0, SPRITE_WIDTH, SPRITE_HEIGHT);
}

void DrawSubSprite(int x, int y, int sprite, int flags, int sx, int sy, int w, int h)
{
    const uint8_t *data = currentSpriteSheet->data + sprite * SPRITE_WIDTH * SPRITE_HEIGHT;

    for (int i = 0; i < w; ++i) {
        for (int j = 0; j < h; ++j) {
            int tx = i;
            int ty = j;
            int zx = w;
            int zy = h;

            if (flags & SPRITE_HFLIP) {
                tx = w - 1 - i;
            }

            if (flags & SPRITE_VFLIP) {
                ty = h - 1 - j;
            }

            if (flags & SPRITE_ROTATE_CW) {
                int z = zx;
                zx = zy;
                zy = z;

                int t = tx;
                tx = ty;
                ty = zy - 1 - t;
            }

            if (flags & SPRITE_ROTATE_CCW) {
                int z = zx;
                zx = zy;
                zy = z;

                int t = ty;
                ty = tx;
                tx = zx - 1 - t;
            }

            int color = data[tx + ty * SPRITE_WIDTH];

            if (flags & SPRITE_ENABLE_MASK) {
                if (color == (flags & 0x0f)) {
                    continue;
                }
            }

            tx += sx;
            ty += sy;

            DrawPixel(tx, ty, color);
        }
    }
}
