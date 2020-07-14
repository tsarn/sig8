#include "sig8_internal.h"

SDL_Window *window = NULL;
SDL_GLContext glContext = NULL;
static int shouldQuit = 0;
static float lastTime = 0.0f;
static float curDelta = 1.0f / 60.0f;

void Initialize(void)
{
    InitializeWindow();
    InitializeScreen();
    InitializeOpenGL();
}

void Finalize(void)
{
    if (glContext) {
        SDL_GL_DeleteContext(glContext);
    }

    if (window) {
        SDL_DestroyWindow(window);
    }

    SDL_Quit();
}

void HandleEvents(void)
{
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT) {
            shouldQuit = 1;
        }

        if (e.type == SDL_WINDOWEVENT) {
            if (e.window.event == SDL_WINDOWEVENT_RESIZED) {
                OnResize();
            }
        }
    }
}

void FrameBegin(void)
{
    float curTime = SDL_GetTicks() / 1000.0f;
    curDelta = curTime - lastTime;
    lastTime = curTime;
    scratchMemorySize = 0;
}

float GetDelta(void)
{
    return curDelta;
}

int ShouldQuit(void)
{
    RedrawScreen();

    // previous frame ends here

    FrameBegin();
    HandleEvents();

    return shouldQuit;
}
