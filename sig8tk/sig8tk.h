#pragma once

#include "sig8.h"
#include <stdlib.h>
#include <string.h>

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

typedef struct {
    int oldResource;
    int newResource;
    uint8_t *data;
} HistoryItem;

typedef struct {
    int size;
    int capacity;
    int cur;
    HistoryItem *data;
} History;

extern Editor editor;
extern SpriteSheet MAIN_SPRITESHEET;

void FillRectR(Rect rect, int color);
Rect AddBorder(Rect rect, int border);
void StrokeRectR(Rect rect, int color);

void DrawToolbar(void);
bool IsMouseOver(Rect rect);
void DrawNumberInput(int x, int y, int *value);
void DrawSlider(int x, int y, int *value);
void DrawIcon(int x, int y, int sprite, int color);

void SpritesInit(void);
void SpritesTick(void);

void HistoryPush(History *history, HistoryItem item);
bool HistoryCanUndo(History *history);
bool HistoryCanRedo(History *history);
HistoryItem HistoryUndo(History *history);
HistoryItem HistoryRedo(History *history);

#define TOOLBAR_SIZE 9
#define TOOLBAR_COLOR DARK_BLUE
