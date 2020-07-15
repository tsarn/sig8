#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef  __cplusplus
extern "C" {
#endif

#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT (SCREEN_WIDTH / 4 * 3)
#define DEFAULT_PIXEL_SIZE 4
#define DEFAULT_SCREEN_WIDTH (SCREEN_WIDTH * DEFAULT_PIXEL_SIZE)
#define DEFAULT_SCREEN_HEIGHT (SCREEN_HEIGHT * DEFAULT_PIXEL_SIZE)

#define N_COLORS 16

#define BLACK 0
#define DARK_BLUE 1
#define DARK_PURPLE 2
#define DARK_GREEN 3
#define BROWN 4
#define DARK_GRAY 5
#define LIGHT_GRAY 6
#define WHITE 7
#define RED 8
#define ORANGE 9
#define YELLOW 10
#define GREEN 11
#define BLUE 12
#define INDIGO 13
#define PINK 14
#define PEACH 15
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
    const uint8_t *data;
} FontDefinition;

typedef const FontDefinition *Font;

extern const Font FONT_5X7;
extern const Font FONT_3X5;

typedef struct {
    int width;
    int height;
    const uint8_t *data;
} SpriteDefinition;

typedef const SpriteDefinition *Sprite;

// System functions
void Initialize(const char *name);
void Finalize(void);
int ShouldQuit(void);
void Quit(void);

// Utility functions
Color ColorFromHex(const char *hex);
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
void RemapColor(int oldColor, int newColor);
void ResetColors(void);
void DrawingArea(int x, int y, int w, int h);
void ResetArea(void);
int GetAreaWidth(void);
int GetAreaHeight(void);
void DrawPixel(int x, int y, int color);
void SetFont(Font font);
void DrawCharacter(int x, int y, int color, char ch);
void DrawString(int x, int y, int color, const char *string);
void DrawSprite(int x, int y, Sprite sprite);
void DrawSubSprite(int x, int y, Sprite sprite, int sx, int sy, int w, int h);
void StrokeRect(int x, int y, int w, int h, int color);
void FillRect(int x, int y, int w, int h, int color);

#ifdef  __cplusplus
};
#endif
