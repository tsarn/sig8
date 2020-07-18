#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef  __cplusplus
extern "C" {
#endif

#define SCREEN_WIDTH 176
#define SCREEN_HEIGHT 128

// Size of game window at startup
#define DEFAULT_PIXEL_SIZE 4
#define DEFAULT_SCREEN_WIDTH (SCREEN_WIDTH * DEFAULT_PIXEL_SIZE)
#define DEFAULT_SCREEN_HEIGHT (SCREEN_HEIGHT * DEFAULT_PIXEL_SIZE)

#define SPRITE_SHEET_SIZE 256
#define SPRITE_WIDTH 8
#define SPRITE_HEIGHT 8

// PICO-8 color palette, licensed under CC-0
// Colors themselves are defined in graphics.c
#define N_COLORS 16
#define BLACK 0
#define DARK_BLUE 1
#define DARK_PURPLE 2
#define DARK_GREEN 3
#define BROWN 4
#define DARK_GRAY 5
#define LIGHT_GRAY 6
#define RED 7
#define ORANGE 8
#define YELLOW 9
#define GREEN 10
#define BLUE 11
#define INDIGO 12
#define PINK 13
#define PEACH 14
#define WHITE 15
#define TRANSPARENT (-1)

typedef enum {
    MOUSE_LEFT = 1,
    MOUSE_MIDDLE = 2,
    MOUSE_RIGHT = 3,
} MouseButton;

typedef struct {
    int x, y;
} Position;

typedef struct {
    uint8_t r, g, b, a;
} Color;

typedef struct {
    int x, y;
    int width, height;
} Rect;

typedef enum {
    CURSOR_ARROW,     /**< Arrow */
    CURSOR_IBEAM,     /**< I-beam */
    CURSOR_WAIT,      /**< Wait */
    CURSOR_CROSSHAIR, /**< Crosshair */
    CURSOR_WAITARROW, /**< Small wait cursor (or Wait if not available) */
    CURSOR_SIZENWSE,  /**< Double arrow pointing northwest and southeast */
    CURSOR_SIZENESW,  /**< Double arrow pointing northeast and southwest */
    CURSOR_SIZEWE,    /**< Double arrow pointing west and east */
    CURSOR_SIZENS,    /**< Double arrow pointing north and south */
    CURSOR_SIZEALL,   /**< Four pointed arrow pointing north, south, east, and west */
    CURSOR_NO,        /**< Slashed circle or crossbones */
    CURSOR_HAND,      /**< Hand */
} CursorShape;

typedef struct {
    int firstCharCode;
    int lastCharCode;
    int width;
    int height;
    int horizontalStep;
    int verticalStep;
    const uint8_t *data;
} FontDefinition;

typedef const FontDefinition *Font;

extern const Font FONT_5X7;
extern const Font FONT_3X5;

typedef struct {
    const uint8_t *data;
} SpriteSheetDefinition;

typedef const SpriteSheetDefinition *SpriteSheet;

/*
 * System functions
 */

void Initialize(const char *name);
void Finalize(void);
int ShouldQuit(void);
void Quit(void);
void SetCursorShape(CursorShape cursor);

/*
 * Utility functions
 */

Color ColorFromHex(const char *hex);
void* TempAlloc(size_t n);
char *Format(const char *fmt, ...);
float GetDelta(void);
bool IsLightColor(int color);

/*
 * Input functions
 */

bool KeyPressed(const char *key);
bool KeyJustPressed(const char *key);
bool KeyJustReleased(const char *key);
char GetJustPressedKey(void);
Position GetMousePosition(void);
bool MousePressed(MouseButton button);
bool MouseJustPressed(MouseButton button);
bool MouseJustReleased(MouseButton button);

/*
 * Drawing functions
 */

void ClearScreen(int color);
void RemapColor(int oldColor, int newColor);
void ResetColors(void);
void DrawPixel(int x, int y, int color);
void SetFont(Font font);
void DrawCharacter(int x, int y, int color, char ch);
void DrawString(int x, int y, int color, const char *string);
void StrokeRect(int x, int y, int w, int h, int color);
void FillRect(int x, int y, int w, int h, int color);
void DrawLine(int x0, int y0, int x1, int y1, int color);

#ifdef  __cplusplus
};
#endif
