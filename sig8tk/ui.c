#include "sig8tk.h"

Editor editor;
SpriteSheet MAIN_SPRITESHEET;

void FillRectR(Rect rect, int color)
{
    FillRect(rect.x, rect.y, rect.width, rect.height, color);
}

void FillRectRB(Rect rect, int color, int borderColor)
{
    StrokeRect(rect.x - 1, rect.y - 1, rect.width + 2, rect.height + 2, borderColor);
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
    DrawLine(0, TOOLBAR_SIZE, SCREEN_WIDTH - 1, TOOLBAR_SIZE, BLACK);

    for (int i = 0; i < NUMBER_OF_EDITORS; ++i) {
        Rect rect = {
                .x = 3 + 9 * i,
                .y = 1,
                .width = 7,
                .height = 7
        };

        if (IsMouseOver(rect)) {
            SetCursorShape(CURSOR_HAND);
        }

        if (editor == i) {
            RemapColor(WHITE, RED);
        } else {
            RemapColor(WHITE, GRAY);
        }

        DrawSprite(rect.x, rect.y, i, SPRITE_MASK_COLOR(BLACK));
        ResetColors();
    }
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
