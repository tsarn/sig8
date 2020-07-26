#pragma once

#include "sig8_internal.h"
#include "stb_image_write.h"

#define SPR_X 16
#define SPR_Y 16

typedef struct {
    int x, y;
    int width, height;
} Rect;

typedef struct {
    uint8_t *data;
} HistoryItem;

typedef struct {
    int size;
    int capacity;
    int cur;
    HistoryItem *data;
} History;

typedef struct {
    void (*handler)();
    const char *hint;
    const char *shortcut;
    int sprite;
} Button;

extern const char *sig8_StatusLine;

void sig8_FillRectR(Rect rect, int color);
Rect sig8_AddBorder(Rect rect, int border);
void sig8_StrokeRectR(Rect rect, int color);

bool sig8_IsMouseOver(Rect rect);
void sig8_DrawNumberInput(int x, int y, int *value);
void sig8_DrawSlider(int x, int y, int *value);
void sig8_DrawIcon(int x, int y, int sprite, int color);
void sig8_DrawSpriteSheet(int x, int y, SpriteSheet spriteSheet, int region, int *selected);
bool sig8_DrawButton(int x, int y, Button button, bool pressed);

void sig8_HistoryClear(History *history);
void sig8_HistoryPush(History *history, HistoryItem item);
bool sig8_HistoryCanUndo(History *history);
bool sig8_HistoryCanRedo(History *history);
HistoryItem sig8_HistoryUndo(History *history);
HistoryItem sig8_HistoryRedo(History *history);
