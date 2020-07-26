#pragma once

#include "sig8_internal.h"

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

void sig8_FillRectR(Rect rect, int color);
Rect sig8_AddBorder(Rect rect, int border);
void sig8_StrokeRectR(Rect rect, int color);

void sig8_DrawToolbar(void);
bool sig8_IsMouseOver(Rect rect);
void sig8_DrawNumberInput(int x, int y, int *value);
void sig8_DrawSlider(int x, int y, int *value);
void sig8_DrawIcon(int x, int y, int sprite, int color);

void sig8_HistoryClear(History *history);
void sig8_HistoryPush(History *history, HistoryItem item);
bool sig8_HistoryCanUndo(History *history);
bool sig8_HistoryCanRedo(History *history);
HistoryItem sig8_HistoryUndo(History *history);
HistoryItem sig8_HistoryRedo(History *history);

#define TOOLBAR_SIZE 9
#define TOOLBAR_COLOR DARK_BLUE
