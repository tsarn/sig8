#include "sig8_internal.h"

uint8_t keyboardState[KEYBOARD_STATE_SIZE];
uint8_t mouseState[MOUSE_STATE_SIZE];
MousePosition mousePosition;
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
        keyboardState[i] &= KEY_PRESSED;
    }

    for (int i = 0; i < MOUSE_STATE_SIZE; ++i) {
        mouseState[i] &= KEY_PRESSED;
    }
}

bool TestKeyState(const char *key, int state)
{
    return (bool)(keyboardState[SDL_GetKeyFromName(key)] & state);
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

MousePosition GetMousePosition(void)
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
