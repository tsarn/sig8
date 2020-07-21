#include "sig8_internal.h"

SDL_Window *window = NULL;
SDL_GLContext glContext = NULL;
bool shouldQuit = false;
static float lastTime = 0.0f;
static float curDelta = 1.0f / 60.0f;
int windowWidth, windowHeight, pixelScale;
float offsetX, offsetY;
SDL_Cursor *cachedCursors[SDL_NUM_SYSTEM_CURSORS];

void Initialize(const char *name)
{
    InitializeWindow(name);
    InitializeScreen();
    InitializeOpenGL();
    InitializeCursors();
    InitializeAudio();
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
            shouldQuit = true;
            break;

        case SDL_WINDOWEVENT:
            if (e.window.event == SDL_WINDOWEVENT_RESIZED) {
                OnResize();
            }
            break;

        case SDL_KEYDOWN:
            if (!e.key.repeat) {
                keyboardState[ConvertKeyCode(e.key.keysym.sym)] = KEY_PRESSED | KEY_JUST_PRESSED;
            }
            break;

        case SDL_KEYUP:
            keyboardState[ConvertKeyCode(e.key.keysym.sym)] = KEY_JUST_RELEASED;
            break;

        case SDL_MOUSEMOTION: {
            float x = (e.motion.x * 1.0f / windowWidth - offsetX) / (1.0f - 2.0f * offsetX);
            float y = (e.motion.y * 1.0f / windowHeight - offsetY) / (1.0f - 2.0f * offsetY);
            if (x >= 0 && x < 1 && y >= 0 && y < 1) {
                mousePosition.x = (int)(x * SCREEN_WIDTH);
                mousePosition.y = (int)(y * SCREEN_HEIGHT);
                isMouseInsideWindow = true;
            } else {
                isMouseInsideWindow = false;
            }
            break;
        }

        case SDL_MOUSEBUTTONDOWN:
            mouseState[e.button.button] = KEY_PRESSED | KEY_JUST_PRESSED;
            break;

        case SDL_MOUSEBUTTONUP:
            mouseState[e.button.button] = KEY_JUST_RELEASED;
            break;
        }

        if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP) {
            if (e.key.keysym.mod & KMOD_CTRL) {
                keyboardState[ConvertKeyCode(e.key.keysym.sym)] |= KEY_CTRL;
            }

            if (e.key.keysym.mod & KMOD_ALT) {
                keyboardState[ConvertKeyCode(e.key.keysym.sym)] |= KEY_ALT;
            }

            if (e.key.keysym.mod & KMOD_SHIFT) {
                keyboardState[ConvertKeyCode(e.key.keysym.sym)] |= KEY_SHIFT;
            }
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

bool Tick(void)
{
    RedrawScreen();

    // previous frame ends here

    UpdateDelta();
    ResetScratchMemory();
    FlushInputs();
    HandleEvents();
    AudioFrameCallback();
    MusicFrameCallback();

    return !shouldQuit;
}

void Quit(void)
{
    shouldQuit = true;
}

void InitializeCursors(void)
{
    for (int i = 0; i < SDL_NUM_SYSTEM_CURSORS; ++i) {
        cachedCursors[i] = SDL_CreateSystemCursor(i);
    }
}

void SetCursorShape(CursorShape cursor)
{
    SDL_SetCursor(cachedCursors[cursor]);
}
