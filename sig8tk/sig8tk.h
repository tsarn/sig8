#pragma once

#include "sig8.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

#define MAX_LINE_LENGTH 4096
#define MAX_RESOURCE_NAME 32
#define MAX_SPRITE_DIMENSION 32

// Color scheme

#define BACKGROUND_COLOR DARK_BLUE
#define TOOLBAR_COLOR LIGHT_GRAY
#define ICON_COLOR INDIGO

// Resources

typedef enum {
    RESOURCE_NONE = 0,
    RESOURCE_SPRITE,
//    RESOURCE_TILEMAP,
//    RESOURCE_SFX,
//    RESOURCE_MUSIC,
//    RESOURCE_FONT,
} ResourceType;

typedef struct {
    int width;
    int height;
    int frames;
    uint8_t *data;
} ResourceSprite;

typedef struct {
    ResourceType type;
    char name[MAX_RESOURCE_NAME];
    union {
        ResourceSprite sprite;
    };
} Resource;


void WriteResource(const Resource *resource, FILE *file);
Resource *ReadResources(FILE *file);

// Sprite editor

void DrawSpriteEditor(void);
void InitSpriteEditor(void);
