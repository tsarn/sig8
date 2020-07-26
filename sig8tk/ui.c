#include "sig8tk.h"

Editor editor;
SpriteSheet MAIN_SPRITESHEET;

void FillRectR(Rect rect, int color)
{
    FillRect(rect.x, rect.y, rect.width, rect.height, color);
}

Rect AddBorder(Rect rect, int border)
{
    return (Rect){
        .x = rect.x - border,
        .y = rect.y - border,
        .width = rect.width + 2 * border,
        .height = rect.height + 2 * border
    };
}

void StrokeRectR(Rect rect, int color)
{
    StrokeRect(rect.x, rect.y, rect.width, rect.height, color);
}

void DrawToolbar(void)
{
    FillRect(0, 0, SCREEN_WIDTH, TOOLBAR_SIZE, TOOLBAR_COLOR);

    for (int i = 0; i < NUMBER_OF_EDITORS; ++i) {
        Rect r = {
                .x = 3 + 9 * i,
                .y = 1,
                .width = 7,
                .height = 7
        };

        if (IsMouseOver(r)) {
            SetCursorShape(CURSOR_HAND);
        }

        if (editor == (Editor)i) {
            DrawIcon(r.x, r.y, i, RED);
        } else {
            DrawIcon(r.x, r.y, i, GRAY);
        }
    }

    FillRect(0, SCREEN_HEIGHT - TOOLBAR_SIZE, SCREEN_WIDTH, TOOLBAR_SIZE, TOOLBAR_COLOR);
}

void DrawNumberInput(int x, int y, int *value)
{
    Rect rect = {
            .x = x,
            .y = y,
            .width = 15,
            .height = 7
    };

    if (IsMouseOver(rect)) {
        SetCursorShape(CURSOR_HAND);
        int delta = 1;
        if (KeyPressed("Ctrl")) {
            delta = 10;
        }

        if (MouseJustPressed(MOUSE_LEFT)) {
            *value += delta;
        }

        if (MouseJustPressed(MOUSE_RIGHT)) {
            *value -= delta;
        }

        *value = Modulo(*value, 256);
    }

    FillRectR(rect, BLACK);
    SetFont(FONT_3X5);
    DrawString(rect.x + 2, y + 6, RED, Format("%03d", *value));
    SetFont(FONT_ASEPRITE);
}

void DrawSlider(int x, int y, int *value)
{
    FillRect(x, y + 2, 27, 2, WHITE);
    StrokeRect(x, y + 1, 27, 4, BLACK);
    for (int i = 0; i < 4; ++i) {
        Rect r = {
                .x = x + 7 * i,
                .y = y,
                .width = 6,
                .height = 6
        };

        if (*value == i) {
            StrokeRectR(r, BLACK);
            FillRectR(AddBorder(r, -1), WHITE);
        }

        if (IsMouseOver(r)) {
            SetCursorShape(CURSOR_HAND);
            if (MousePressed(MOUSE_LEFT)) {
                *value = i;
            }
        }
    }
}

void DrawIcon(int x, int y, int sprite, int color)
{
    RemapColor(WHITE, color);
    DrawSprite(x, y, sprite, SPRITE_MASK_COLOR(BLACK));
    ResetColors();
}

bool IsMouseOver(Rect rect)
{
    Position pos = GetMousePosition();

    if (pos.x < rect.x || pos.y < rect.y) {
        return false;
    }

    if (pos.x >= rect.x + rect.width || pos.y >= rect.y + rect.height) {
        return false;
    }

    return true;
}
