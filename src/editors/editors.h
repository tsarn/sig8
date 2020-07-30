#pragma once

#include "sig8_internal.h"
#include "stb_image_write.h"

#define swap(x, y) ((x) ^= (y), (y) ^= (x), (x) ^= (y))
#define min(x, y) (((x) < (y)) ? (x) : (y))
#define max(x, y) (((x) > (y)) ? (x) : (y))
#define diff(x, y) (max(x, y) - min(x, y))

typedef struct {
    int x, y;
    int width, height;
} Rect;

typedef struct {
    bool active;
    bool resizing;
    int x1, y1;
    int x2, y2;
} Selection;

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
    const char *hint;
    const char *shortcut;
    int sprite;
} Button;

extern const char *sig8_StatusLine;
extern const ManagedResource *sig8_Editing;

void sig8_FillRectR(Rect rect, int color);
Rect sig8_AddBorder(Rect rect, int border);
void sig8_StrokeRectR(Rect rect, int color);

bool sig8_IsMouseOver(Rect rect);
void sig8_DrawNumberInput(int x, int y, int *value);
void sig8_DrawSlider(int x, int y, int *value);
void sig8_DrawIcon(int x, int y, int sprite, int color);
void sig8_DrawSpriteSheet(int x, int y, SpriteSheet spriteSheet, int region, int *selected);
bool sig8_DrawButton(int x, int y, Button button, bool pressed);
void sig8_Selection(Selection *selection, int x, int y);

void sig8_HistoryClear(void);
void sig8_Undo(void);
void sig8_Redo(void);
void sig8_BeginUndoableAction(void);
void sig8_EndUndoableAction(void);
