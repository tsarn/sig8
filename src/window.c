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
        switch (e.type) {
        case SDL_QUIT:
            shouldQuit = 1;
            break;

        case SDL_WINDOWEVENT:
            if (e.window.event == SDL_WINDOWEVENT_RESIZED) {
                OnResize();
            }
            break;

        case SDL_KEYDOWN:
            keyboardState[ConvertKeyCode(e.key.keysym.sym)] = KEY_PRESSED | KEY_JUST_PRESSED;
            if (e.key.repeat) {
                keyboardState[ConvertKeyCode(e.key.keysym.sym)] |= KEY_IS_REPEAT;
            }
            break;

        case SDL_KEYUP:
            keyboardState[ConvertKeyCode(e.key.keysym.sym)] = KEY_JUST_RELEASED;
            break;
        }
    }
}

void UpdateDelta(void)
{
    float curTime = SDL_GetTicks() / 1000.0f;
    curDelta = curTime - lastTime;
    lastTime = curTime;
}

void ResetScratchMemory(void)
{
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

    UpdateDelta();
    ResetScratchMemory();
    FlushInputs();
    HandleEvents();

    return shouldQuit;
}

void Quit(void)
{
    shouldQuit = 1;
}
