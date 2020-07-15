#pragma once

#include "sig8.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

// Color scheme

#define BACKGROUND_COLOR DARK_BLUE
#define TOOLBAR_COLOR LIGHT_GRAY
#define ICON_COLOR INDIGO

// Resources

typedef enum {
    RESOURCE_SPRITE = 1,
    RESOURCE_TILEMAP,
    RESOURCE_SFX,
    RESOURCE_MUSIC,
    RESOURCE_FONT,
} ResourceType;

typedef struct {
    int width;
    int height;
    int frames;
    uint8_t *data;
} ResourceSprite;

typedef struct {
    ResourceType type;
    const char *name;
    union {
        ResourceSprite sprite;
    };
} Resource;

// Sprite editor

typedef struct {
    ResourceSprite sprite;
    int fg, bg;
} SpriteEditor;

extern SpriteEditor spriteEditor;

void DrawSpriteEditor(void);
void InitSpriteEditor(void);
