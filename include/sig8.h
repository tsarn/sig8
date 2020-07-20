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

/*
 * Sprite flags
 *
 * When drawing a sprite it is possible to specify
 * the following flags. Most of them are self-explanatory.
 * Flips are done before the rotations.
 *
 * A color specified in the lower four bits
 * can be replaced with transparency like so:
 * SPRITE_MASK_COLOR(<color goes here>)
 */

#define SPRITE_HFLIP 0x10
#define SPRITE_VFLIP 0x20
#define SPRITE_ROTATE_CW 0x40
#define SPRITE_ROTATE_180 (SPRITE_HFLIP | SPRITE_VFLIP)
#define SPRITE_ROTATE_CCW (ROTATE_CW | ROTATE_180)
#define SPRITE_ENABLE_MASK 0x80
#define SPRITE_MASK_COLOR(color) (SPRITE_ENABLE_MASK | (color))

#define TILEMAP_WIDTH 256
#define TILEMAP_HEIGHT 256

#define SOUND_CHANNELS 8
#define MAX_ENVELOPE_LENGTH 16

// Colors themselves are defined in graphics.c
#define N_COLORS 16
#define BLACK 0
#define RED 1
#define ORANGE 2
#define YELLOW 3
#define GREEN 4
#define BLUE 5
#define PINK 6
#define PEACH 7
#define PURPLE 8
#define DARK_RED 9
#define BROWN 10
#define DARK_GREEN 11
#define DARK_BLUE 12
#define INDIGO 13
#define GRAY 14
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
    int lineHeight;
    bool isMono;
    const uint8_t *data;
} FontDefinition;

typedef const FontDefinition *Font;

extern const Font FONT_5X7;
extern const Font FONT_3X5;
extern const Font FONT_ASEPRITE;

typedef const uint8_t *SpriteSheet;
typedef uint8_t *TileMap;

/* Music */

typedef enum {
    STOP_NOTE = 0,
    A0, AS0, B0,
    C1, C1S, D1, D1S, E1, F1, F1S, G1, G1S, A1, A1S, B1,
    C2, C2S, D2, D2S, E2, F2, F2S, G2, G2S, A2, A2S, B2,
    C3, C3S, D3, D3S, E3, F3, F3S, G3, G3S, A3, A3S, B3,
    C4, C4S, D4, D4S, E4, F4, F4S, G4, G4S, A4, A4S, B4,
    C5, C5S, D5, D5S, E5, F5, F5S, G5, G5S, A5, A5S, B5,
    C6, C6S, D6, D6S, E6, F6, F6S, G6, G6S, A6, A6S, B6,
    C7, C7S, D7, D7S, E7, F7, F7S, G7, G7S, A7, A7S, B7,
    C8,
} Note;

typedef enum {
    SQUARE_WAVE,
    SAWTOOTH_WAVE,
    SINE_WAVE,
    TRIANGLE_WAVE,
    NOISE
} Wave;

typedef enum {
    // Volume (0..63), any wave
    ENVELOP_VOLUME,

    // Duty cycle, square wave
    // 0 is 12.5%, 1 is 25%, 2 is 50%, 3 is 75%
    ENVELOP_DUTY_CYCLE,

    // Pitch, any wave except noise
    // Adds pitch, 1/16th of a half tone per unit
    ENVELOP_PITCH,

    NUMBER_OF_ENVELOPES

} EnvelopeType;

typedef struct {
    int length;
    int loopBegin;
    int loopEnd;
    int value[MAX_ENVELOPE_LENGTH];
} Envelope;

typedef struct {
    Wave wave;
    Envelope envelopes[NUMBER_OF_ENVELOPES];
    float volume;
} Instrument;

typedef struct {
    Note notes[SOUND_CHANNELS];
    Instrument instruments[SOUND_CHANNELS];
    int duration[SOUND_CHANNELS]; // ticks since the note started playing
    float volume;
} AudioFrame;

/*
 * System functions
 */

void Initialize(const char *name);
void Finalize(void);
bool Tick(void);
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
void DrawString(int x, int y, int color, const char *string);
int MeasureString(const char *string);
void StrokeRect(int x, int y, int w, int h, int color);
void FillRect(int x, int y, int w, int h, int color);
void DrawLine(int x0, int y0, int x1, int y1, int color);

void UseSpriteSheet(SpriteSheet spriteSheet);
void FreeSpriteSheet(SpriteSheet spriteSheet);
void DrawSprite(int x, int y, int sprite, int flags);
void DrawSubSprite(int x, int y, int sprite, int flags, int sx, int sy, int w, int h);
SpriteSheet SpriteSheetFromImage(const char *filename);

/*
 * Audio functions
 */

void SetInstrument(int channel, Instrument instrument);
void PlayNote(int channel, Note note);

#ifdef  __cplusplus
};
#endif
