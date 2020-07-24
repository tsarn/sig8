#pragma once

#include "sig8.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <GLES3/gl3.h>

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

#define MAX_EVENT_HANDLERS 16
#define FRAME_EVENT (-1)

typedef void (*EventCallback)(SDL_Event*);
typedef void (*FrameCallback)(void);

void sig8_InitWindow(const char *name);
void sig8_InitGLES(void);
void sig8_InitScreen(Color *screen);
void sig8_InitAudio(void);
void sig8_InitMusic(void);
void sig8_InitInput(void);
void sig8_InitAlloc(void);

void sig8_RegisterFrameCallback(FrameCallback callback);
void sig8_RegisterEventCallback(int type, EventCallback callback);
bool sig8_HandleEvent(int type, SDL_Event *event);
