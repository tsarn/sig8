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
#define MAX_LAYOUT_NESTING 64
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
#define TRANSPARENT 0xFF

typedef enum {
    MOUSE_LEFT = 1,
    MOUSE_MIDDLE = 2,
    MOUSE_RIGHT = 3,
    MOUSE_HOVER = 7,
} MouseButton;

typedef struct {
    int x, y;
} MousePosition;

typedef struct {
    uint8_t r, g, b, a;
} Color;

typedef struct {
    int x, y;
    int width, height;
} Rect;

typedef enum {
    HALIGN_LEFT = 0,
    HALIGN_CENTER,
    HALIGN_RIGHT
} HAlign;

typedef enum {
    VALIGN_TOP = 0,
    VALIGN_MIDDLE,
    VALIGN_BOTTOM
} VAlign;

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
void SetCursorShape(CursorShape cursor);

// Utility functions
Color ColorFromHex(const char *hex);
void* TempAlloc(size_t n);
char *Format(const char *fmt, ...);
float GetDelta(void);
bool IsLightColor(int color);

// Input functions
bool KeyPressed(const char *key);
bool KeyJustPressed(const char *key);
bool KeyJustReleased(const char *key);
char GetJustPressedKey(void);
MousePosition GetMousePosition(void);
bool MousePressed(MouseButton button);
bool MouseJustPressed(MouseButton button);
bool MouseJustReleased(MouseButton button);

// Drawing functions
void ClearScreen(int color);
void RemapColor(int oldColor, int newColor);
void ResetColors(void);
void DrawPixel(int x, int y, int color);
void SetFont(Font font);
void DrawCharacter(int x, int y, int color, char ch);
void DrawString(int x, int y, int color, const char *string);
void DrawSprite(int x, int y, Sprite sprite);
void DrawSubSprite(int x, int y, Sprite sprite, int sx, int sy, int w, int h);
void StrokeRect(int x, int y, int w, int h, int color);
void FillRect(int x, int y, int w, int h, int color);
void DrawHLine(int x, int y, int w, int color);
void DrawVLine(int x, int y, int h, int color);

void SetArea(int x, int y, int w, int h);
Rect GetArea(void);
void ResetArea(void);
void ResetAreaStack(void);
void PushArea(void);
void PopArea(void);
int GetAreaWidth(void);
int GetAreaHeight(void);

// UI functions
void BeginUI(void);
void BeginMargin(int top, int right, int bottom, int left);
void BeginCenter(int width, int height);
void BeginVBox(int separation);
void BeginHBox(int separation);
void BeginItem(int size);
void ColorLayout(int color);
void EndLayout(void);
int EqualSize(int amount);
bool Button(MouseButton button);
void Text(const char *text, int color, HAlign hAlign, VAlign vAlign);

#ifdef  __cplusplus
};
#endif
