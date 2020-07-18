#pragma once

#include "sig8.h"
#include "sig8tk_resources.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

#define MAX_LINE_LENGTH 4096
#define MAX_RESOURCE_NAME 16
#define MAX_SPRITE_DIMENSION 32

// Layout settings

#define TOOLBAR_HEIGHT 7
#define SIDEBAR_WIDTH 43

// Color scheme

#define BACKGROUND_COLOR DARK_BLUE
#define TOOLBAR_COLOR LIGHT_GRAY
#define ICON_COLOR INDIGO
#define SIDEBAR_COLOR DARK_GRAY

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


void WriteResources(FILE *file);
void ReadResources(FILE *file);
void FreeResources(void);
void EditResource(Resource *resource);
void RemoveResource(Resource *resource);
void CreateResource(const char *name, ResourceType type);

extern Resource *resources;
extern int resourceCount;
extern Resource *editedResource;
extern const char *statusString;

// Generic

void DrawToolbar(void);
void DrawStatusString(void);

// Resource selector

void InitResourceSelector(const char *path);
void DrawResourceSelector(void);

// Sprite editor

void InitSpriteEditor(void);
void DrawSpriteEditor(void);
void InitSprite(ResourceSprite *sprite, int w, int h);
