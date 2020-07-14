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

typedef struct {
    float r, g, b, a;
} FloatColor;

void DrawScreen(void);
void InitializeOpenGL(void);
void OnResize(void);
void UpdateBufferData(void);
FloatColor ColorToFloatColor(Color color);

extern SDL_Window *window;
extern Color screenBuffer[SCREEN_WIDTH * SCREEN_HEIGHT];
extern Color colorMap[N_COLORS];
