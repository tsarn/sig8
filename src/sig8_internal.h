#pragma once

#include "config.h"
#include "sig8.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stddef.h>
#include <time.h>
#include <SDL2/SDL.h>

#ifdef SIG8_USE_GLAD
#include <glad.h>
#else
#include <GLES3/gl3.h>
#endif

#define KEYBOARD_STATE_SIZE 512
#define MOUSE_STATE_SIZE 16
#define KEY_PRESSED 0x01
#define KEY_JUST_PRESSED 0x02
#define KEY_JUST_RELEASED 0x04
#define KEY_IS_REPEAT 0x08
#define KEY_CTRL 0x10
#define KEY_ALT 0x20
#define KEY_SHIFT 0x40
#define KEY_MODS (KEY_CTRL | KEY_ALT | KEY_SHIFT)

#define FRAME_RATE 60
#define SAMPLE_RATE 44100
#define SAMPLE_SIZE 1024
#define PRELOAD_SOUNDS 4
#define SAMPLES_PER_FRAME (SAMPLE_RATE / FRAME_RATE)

#define SPRITESHEET_BYTE_SIZE (SPRITESHEET_SIZE * SPRITE_WIDTH * SPRITE_HEIGHT)
#define TILEMAP_BYTE_SIZE (TILEMAP_WIDTH * TILEMAP_HEIGHT)
#define SOUNDLIB_BYTE_SIZE (SOUNDLIB_SIZE * sizeof(Sound))
#define MUSICLIB_BYTE_SIZE (MUSICLIB_SIZE * sizeof(Track))

#ifdef SIG8_COMPILE_EDITORS
#define MAX_EVENT_HANDLERS 32
#else
#define MAX_EVENT_HANDLERS 16
#endif

#define FRAME_EVENT (-1)
#define EDITOR_ENTER_EVENT (-2)
#define EDITOR_LEAVE_EVENT (-3)

#define RESOURCE_PATH_PREFIX "res://"

typedef enum {
    RESOURCE_SPRITESHEET,
    RESOURCE_TILEMAP,
    RESOURCE_SOUNDLIB,
    RESOURCE_MUSICLIB,
} ResourceType;

typedef void (*EventCallback)(SDL_Event*);
typedef void (*Callback)(void);

void sig8_InitWindow(const char *name);
void sig8_InitGLES(void);
void sig8_InitGLESPixelBuffer(void);
void sig8_InitScreen(Color *screen);
void sig8_InitAudio(void);
void sig8_InitMusic(void);
void sig8_InitInput(void);
void sig8_UpdateScreen(void);
void sig8_LeaveEditor(void);
int sig8_GetPlayingTime(int channel, const Envelope *envelope);
void sig8_GetMusicState(bool *playing, int *track, int *fragment, int *row);

void sig8_RegisterCallback(int type, Callback callback);
void sig8_RegisterEventCallback(int type, EventCallback callback);
bool sig8_EmitEvent(int type, SDL_Event *event);

void *sig8_AllocateResource(ResourceType type, const char *path, int size);
void sig8_FreeResource(void *resource);

static inline int Modulo(int a, int b)
{
    return (a % b + b) % b;
}

static inline int Divide(int a, int b)
{
    return (a - Modulo(a, b)) / b;
}

#ifdef SIG8_COMPILE_EDITORS

#include "sig8_editors_resources.h"

extern SpriteSheet sig8_EDITORS_SPRITESHEET;

#define EDITOR_WIDTH 200
#define EDITOR_HEIGHT 151

typedef struct {
    ResourceType type;
    int size;
    char *path;
    uint8_t *resource;
} ManagedResource;

ManagedResource *sig8_GetManagedResource(uint8_t *resource);

void sig8_SpriteEditorInit(ManagedResource *what);
void sig8_SpriteEditorTick(void);

void sig8_TileEditorInit(ManagedResource *what);
void sig8_TileEditorTick(void);

void sig8_SoundEditorInit(ManagedResource *what);
void sig8_SoundEditorTick(void);

void sig8_MusicEditorInit(ManagedResource *what);
void sig8_MusicEditorTick(void);

#endif
