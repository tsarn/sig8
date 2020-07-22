#include "sig8_internal.h"

uint8_t keyboardState[KEYBOARD_STATE_SIZE];
uint8_t mouseState[MOUSE_STATE_SIZE];
Position mousePosition;
bool isMouseInsideWindow = false;

int ConvertKeyCode(int keyCode)
{
    if (keyCode & (1 << 30)) {
        keyCode ^= (1 << 30);
        keyCode += 0x80;
    }
    return keyCode;
}

void FlushInputs(void)
{
    for (int i = 0; i < KEYBOARD_STATE_SIZE; ++i) {
        keyboardState[i] &= KEY_PRESSED | KEY_MODS;
    }

    for (int i = 0; i < MOUSE_STATE_SIZE; ++i) {
        mouseState[i] &= KEY_PRESSED;
    }
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

    int code = ConvertKeyCode(SDL_GetKeyFromName(key));
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
