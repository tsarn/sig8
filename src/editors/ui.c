#include "editors.h"

SpriteSheet sig8_EDITORS_SPRITESHEET;
const char *sig8_StatusLine = "";

void sig8_FillRectR(Rect rect, int color)
{
    FillRect(rect.x, rect.y, rect.width, rect.height, color);
}

Rect sig8_AddBorder(Rect rect, int border)
{
    return (Rect){
        .x = rect.x - border,
        .y = rect.y - border,
        .width = rect.width + 2 * border,
        .height = rect.height + 2 * border
    };
}

void sig8_StrokeRectR(Rect rect, int color)
{
    StrokeRect(rect.x, rect.y, rect.width, rect.height, color);
}

void sig8_DrawNumberInput(int x, int y, int *value)
{
    Rect rect = {
            .x = x,
            .y = y,
            .width = 15,
            .height = 7
    };

    if (sig8_IsMouseOver(rect)) {
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

    sig8_FillRectR(rect, BLACK);
    UseFont(FONT_TINY);
    DrawString(rect.x + 2, y + 6, RED, Format("%03d", *value));
    UseFont(FONT_MEDIUM);
}

void sig8_DrawSlider(int x, int y, int *value)
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
            sig8_StrokeRectR(r, BLACK);
            sig8_FillRectR(sig8_AddBorder(r, -1), WHITE);
        }

        if (sig8_IsMouseOver(r)) {
            SetCursorShape(CURSOR_HAND);
            if (MousePressed(MOUSE_LEFT)) {
                *value = i;
            }
        }
    }
}

void sig8_DrawIcon(int x, int y, int sprite, int color)
{
    RemapColor(WHITE, color);
    DrawSprite(x, y, sprite);
    ResetColors();
}

bool sig8_IsMouseOver(Rect rect)
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

void sig8_DrawSpriteSheet(int x, int y, SpriteSheet spriteSheet, int region, int *selected)
{
    int selectedX = *selected % SPR_X;
    int selectedY = *selected / SPR_X;
    int width = region * SPRITE_WIDTH;
    int height = region * SPRITE_HEIGHT;

    Rect rect = {
            .x = x,
            .y = y,
            .width = SPR_X * SPRITE_WIDTH,
            .height = SPR_Y * SPRITE_HEIGHT,
    };

    sig8_FillRectR(sig8_AddBorder(rect, 1), BLACK);
    UseSpriteSheet(spriteSheet);

    for (int j = 0; j < SPR_Y; ++j) {
        for (int i = 0; i < SPR_X; ++i) {
            int idx = i + j * SPR_X;
            Rect r = {
                    .x = rect.x + i * SPRITE_WIDTH,
                    .y = rect.y + j * SPRITE_HEIGHT,
                    .width = SPRITE_WIDTH,
                    .height = SPRITE_HEIGHT
            };
            DrawSprite(r.x, r.y, idx);

            if (sig8_IsMouseOver(r)) {
                SetCursorShape(CURSOR_HAND);
                if (MousePressed(MOUSE_LEFT)) {
                    selectedX = i - region / 2;
                    selectedY = j - region / 2;
                }
            }
        }
    }

    UseSpriteSheet(sig8_EDITORS_SPRITESHEET);

    if (selectedX < 0) selectedX = 0;
    if (selectedY < 0) selectedY = 0;
    if (selectedX + region > SPR_X) selectedX = SPR_X - region;
    if (selectedY + region > SPR_Y) selectedY = SPR_Y - region;

    *selected = selectedX + selectedY * SPR_X;

    Rect r = {
            .x = rect.x + selectedX * SPRITE_WIDTH,
            .y = rect.y + selectedY * SPRITE_HEIGHT,
            .width = width,
            .height = height
    };
    sig8_StrokeRectR(sig8_AddBorder(r, 2), WHITE);
    sig8_StrokeRectR(sig8_AddBorder(r, 1), BLACK);
}

bool sig8_DrawButton(int x, int y, Button button, bool pressed)
{
    Rect r = {
            .x = x,
            .y = y,
            .width = SPRITE_WIDTH - 1,
            .height = SPRITE_HEIGHT - 1
    };

    if (!pressed) {
        sig8_DrawIcon(r.x, r.y + 1, button.sprite, BLACK);
        sig8_DrawIcon(r.x, r.y, button.sprite, GRAY);
    } else {
        sig8_DrawIcon(r.x, r.y + 1, button.sprite, WHITE);
    }

    if (sig8_IsMouseOver(r)) {
        SetCursorShape(CURSOR_HAND);
        sig8_StatusLine = button.hint;

        if (MouseJustPressed(MOUSE_LEFT)) {
            return true;
        }
    }

    if (button.shortcut) {
        if (KeyJustPressed(button.shortcut)) {
            return true;
        }
    }

    return false;
}
