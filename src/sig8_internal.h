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

typedef struct {
    float r, g, b, a;
} FloatColor;

void InitializeWindow(const char *name);
void InitializeOpenGL(void);
void InitializeScreen(void);
void InitializeCursors(void);
void InitializeAudio(void);

void HandleEvents(void);
void RedrawScreen(void);

void OnResize(void);
void UpdateBufferData(void);
FloatColor ColorToFloatColor(Color color);

int ConvertKeyCode(int keyCode);
void FlushInputs(void);

void AudioFrameCallback(void);
float GetNoteFrequency(Note note);

extern int windowWidth, windowHeight, pixelScale;
extern float offsetX, offsetY;
extern bool shouldQuit;
extern SDL_Window *window;
extern SDL_GLContext glContext;
extern SDL_Cursor *cachedCursors[SDL_NUM_SYSTEM_CURSORS];

extern Color screenBuffer[SCREEN_WIDTH * SCREEN_HEIGHT];
extern Color colorMap[N_COLORS];
extern int paletteMap[N_COLORS];
extern const char *colorNames[N_COLORS];
extern Font currentFont;
extern SpriteSheet currentSpriteSheet;

extern char *scratchMemory;
extern size_t scratchMemorySize;
extern size_t scratchMemoryCapacity;

extern uint8_t keyboardState[KEYBOARD_STATE_SIZE];
extern uint8_t mouseState[MOUSE_STATE_SIZE];
extern Position mousePosition;
extern bool isMouseInsideWindow;
