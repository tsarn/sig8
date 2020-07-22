#include "sig8_internal.h"

static uint8_t keyboardState[KEYBOARD_STATE_SIZE];
static uint8_t mouseState[MOUSE_STATE_SIZE];
static Position mousePosition;
static bool isMouseInsideWindow = false;

static int Convert(int keyCode)
{
    if (keyCode & (1 << 30)) {
        keyCode ^= (1 << 30);
        keyCode += 0x80;
    }
    return keyCode;
}

static void HandleEvent(SDL_Event *event)
{
    switch (event->type) {
    case SDL_KEYDOWN:
        if (!event->key.repeat) {
            keyboardState[Convert(event->key.keysym.sym)] = KEY_PRESSED | KEY_JUST_PRESSED;
        }
        break;

    case SDL_KEYUP:
        keyboardState[Convert(event->key.keysym.sym)] = KEY_JUST_RELEASED;
        break;

    case SDL_MOUSEMOTION: {
        float x = *(float*)event->user.data1;
        float y = *(float*)event->user.data2;
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
        mouseState[event->button.button] = KEY_PRESSED | KEY_JUST_PRESSED;
        break;

    case SDL_MOUSEBUTTONUP:
        mouseState[event->button.button] = KEY_JUST_RELEASED;
        break;
    }

    if (event->type == SDL_KEYDOWN || event->type == SDL_KEYUP) {
        if (event->key.keysym.mod & KMOD_CTRL) {
            keyboardState[Convert(event->key.keysym.sym)] |= KEY_CTRL;
        }

        if (event->key.keysym.mod & KMOD_ALT) {
            keyboardState[Convert(event->key.keysym.sym)] |= KEY_ALT;
        }

        if (event->key.keysym.mod & KMOD_SHIFT) {
            keyboardState[Convert(event->key.keysym.sym)] |= KEY_SHIFT;
        }
    }
}

static void FlushInputs(void)
{
    for (int i = 0; i < KEYBOARD_STATE_SIZE; ++i) {
        keyboardState[i] &= KEY_PRESSED | KEY_MODS;
    }

    for (int i = 0; i < MOUSE_STATE_SIZE; ++i) {
        mouseState[i] &= KEY_PRESSED;
    }
}

void sig8_InitInput(void)
{
    sig8_RegisterFrameCallback(FlushInputs);
    sig8_RegisterEventCallback(SDL_KEYDOWN, HandleEvent);
    sig8_RegisterEventCallback(SDL_KEYUP, HandleEvent);
    sig8_RegisterEventCallback(SDL_MOUSEMOTION, HandleEvent);
    sig8_RegisterEventCallback(SDL_MOUSEBUTTONDOWN, HandleEvent);
    sig8_RegisterEventCallback(SDL_MOUSEBUTTONUP, HandleEvent);
}

static bool TestKeyState(const char *key, int state)
{
    int mod = 0;

    while (1) {
        if (strncmp("Ctrl+", key, 5) == 0) {
            mod |= KEY_CTRL;
            key += 5;
            continue;
        }

        if (strncmp("Alt+", key, 4) == 0) {
            mod |= KEY_ALT;
            key += 4;
            continue;
        }

        if (strncmp("Shift+", key, 6) == 0) {
            mod |= KEY_SHIFT;
            key += 6;
            continue;
        }

        break;
    }

    int code = Convert(SDL_GetKeyFromName(key));
    return (bool)(keyboardState[code] & state) &&
            (bool)((keyboardState[code] & KEY_MODS) == mod);
}

bool KeyPressed(const char *key)
{
    return TestKeyState(key, KEY_PRESSED);
}

bool KeyJustPressed(const char *key)
{
    return TestKeyState(key, KEY_JUST_PRESSED);
}

bool KeyJustReleased(const char *key)
{
    return TestKeyState(key, KEY_JUST_RELEASED);
}

char GetJustPressedKey(void)
{
    for (int i = 1; i < 128; ++i) {
        if (keyboardState[i] & (KEY_CTRL | KEY_ALT)) {
            continue;
        }

        if (keyboardState[i] & KEY_JUST_PRESSED) {
            return (char)toupper(i);
        }
    }
    return 0;
}

Position GetMousePosition(void)
{
    return mousePosition;
}

bool TestMouseState(MouseButton button, int state)
{
    return isMouseInsideWindow && (bool)(mouseState[button] & state);
}

bool MousePressed(MouseButton button)
{
    return TestMouseState(button, KEY_PRESSED);
}

bool MouseJustPressed(MouseButton button)
{
    return TestMouseState(button, KEY_JUST_PRESSED);
}

bool MouseJustReleased(MouseButton button)
{
    return TestMouseState(button, KEY_JUST_RELEASED);
}
