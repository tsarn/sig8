#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef  __cplusplus
extern "C" {
#endif

#define TRANSPARENT 0xFF

typedef enum {
    MOUSE_LEFT = 1,
    MOUSE_MIDDLE = 2,
    MOUSE_RIGHT = 3,
} MouseButton;

typedef struct {
    int x, y;
} MousePosition;

typedef struct {
    uint8_t r, g, b, a;
} Color;

typedef struct {
    uint8_t firstCharCode;
    uint8_t lastCharCode;
    uint8_t width;
    uint8_t height;
    uint8_t horizontalStep;
    uint8_t verticalStep;
    const uint8_t *fontData;
} FontDefinition;

typedef const FontDefinition *Font;

extern const Font font5x7;
extern const Font font3x5;

typedef struct {
    int width;
    int height;
    const uint8_t *spriteData;
} SpriteDefinition;

typedef const SpriteDefinition *Sprite;

extern const Sprite icon;

// System functions
void Initialize(void);
void Finalize(void);
int ShouldQuit(void);
void Quit(void);

// Utility functions
Color ColorFromHex(const char *hex);
int GetScreenWidth(void);
int GetScreenHeight(void);
void* TempAlloc(size_t n);
char *Format(const char *fmt, ...);
float GetDelta(void);

// Input functions
bool KeyPressed(const char *key);
bool KeyJustPressed(const char *key);
bool KeyJustReleased(const char *key);
MousePosition GetMousePosition(void);
bool MousePressed(MouseButton button);
bool MouseJustPressed(MouseButton button);
bool MouseJustReleased(MouseButton button);

// Drawing functions
void ClearScreen(int color);
void DrawPixel(int x, int y, int color);
void SetFont(Font font);
void DrawCharacter(int x, int y, int color, char ch);
void DrawString(int x, int y, int color, const char *string);
void DrawSprite(int x, int y, Sprite sprite);
void DrawSubSprite(int x, int y, Sprite sprite, int sx, int sy, int w, int h);

#ifdef  __cplusplus
};
#endif
