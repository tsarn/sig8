#pragma once

#include "sig8.h"
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <SDL2/SDL.h>
#include "glad.h"

#define KEYBOARD_STATE_SIZE 512
#define KEY_PRESSED 0x01
#define KEY_JUST_PRESSED 0x02
#define KEY_JUST_RELEASED 0x04
#define KEY_IS_REPEAT 0x08

typedef struct {
    float r, g, b, a;
} FloatColor;

void InitializeWindow(void);
void InitializeOpenGL(void);
void InitializeScreen(void);

void HandleEvents(void);
void RedrawScreen(void);

void OnResize(void);
void UpdateBufferData(void);
FloatColor ColorToFloatColor(Color color);

int ConvertKeyCode(int keyCode);
void FlushInputs(void);

extern SDL_Window *window;
extern SDL_GLContext glContext;
extern Color screenBuffer[SCREEN_WIDTH * SCREEN_HEIGHT];
extern Color colorMap[N_COLORS];
extern char *scratchMemory;
extern size_t scratchMemorySize;
extern size_t scratchMemoryCapacity;
extern uint8_t keyboardState[KEYBOARD_STATE_SIZE];
