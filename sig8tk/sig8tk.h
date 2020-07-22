#pragma once

#include "sig8.h"

typedef enum {
    SPRITE_EDITOR = 0,
    TILE_EDITOR,
    SOUND_EDITOR,
    MUSIC_EDITOR,

    NUMBER_OF_EDITORS
} Editor;

typedef struct {
    int x, y;
    int width, height;
} Rect;

extern Editor editor;
extern SpriteSheet MAIN_SPRITESHEET;

void FillRectR(Rect rect, int color);
void FillRectRB(Rect rect, int color, int borderColor);
Rect AddBorder(Rect rect, int border);
void StrokeRectR(Rect rect, int color);

void DrawToolbar(void);
bool IsMouseOver(Rect rect);

void SpritesInit(void);
void SpritesTick(void);

#define TOOLBAR_SIZE 9
#define TOOLBAR_COLOR DARK_BLUE
