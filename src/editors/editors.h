#pragma once

#include "sig8_internal.h"
#include "stb_image_write.h"

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

void sig8_HistoryClear(void);
void sig8_Undo(void);
void sig8_Redo(void);
void sig8_BeginUndoableAction(void);
void sig8_EndUndoableAction(void);
