#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef  __cplusplus
extern "C" {
#endif

// For most intents and purposes these are constants
#define SCREEN_WIDTH (GetScreenWidth())
#define SCREEN_HEIGHT (GetScreenHeight())
#define PALETTE_SIZE (GetPalette().size)
#define MAX_PALETTE_SIZE 32

// Size of the game window at startup
#define DEFAULT_PIXEL_SIZE 4
#define DEFAULT_WINDOW_WIDTH (SCREEN_WIDTH * DEFAULT_PIXEL_SIZE)
#define DEFAULT_WINDOW_HEIGHT (SCREEN_HEIGHT * DEFAULT_PIXEL_SIZE)

#define SPRITE_WIDTH 8
#define SPRITE_HEIGHT 8

#define SPRITESHEET_WIDTH 16
#define SPRITESHEET_HEIGHT 16
#define SPRITESHEET_SIZE (SPRITESHEET_WIDTH * SPRITESHEET_HEIGHT)

#define TILEMAP_WIDTH 256
#define TILEMAP_HEIGHT 256

#define SOUNDLIB_SIZE 64

#define SOUND_CHANNELS 8
#define ENVELOPE_LENGTH 32

#ifndef DISABLE_COLOR_DEFINES
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
#endif

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
    uint8_t r, g, b;
} Color;

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

typedef struct {
    int size;
    Color *colors;
} Palette;

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
    NOISE,

    NUMBER_OF_WAVES
} Wave;

#define ENVELOPE_VOLUME_MAX 15
#define ENVELOPE_DUTY_CYCLE_MAX 15

typedef enum {
    // Volume (0..15), any wave
            ENVELOPE_VOLUME,
    // Pitch, any wave except noise
    // Adds pitch, 1/16th of a semi tone per unit
            ENVELOPE_PITCH,

    // Arpeggio, any wave except noise
    // Shifts note in increments of semi tones
            ENVELOPE_ARPEGGIO,

    // Duty cycle (0..15), square wave
    // 0 is 1/32 duty cycle, 15 is 1/2 duty cycle
            ENVELOPE_DUTY_CYCLE,

    NUMBER_OF_ENVELOPES

} EnvelopeType;

typedef struct {
    int8_t loopBegin;
    int8_t loopEnd;
    int8_t value[ENVELOPE_LENGTH];
} Envelope;

typedef struct {
    Wave wave;
    Envelope envelopes[NUMBER_OF_ENVELOPES];
    uint8_t volume; // volume in range [0..255]
    uint8_t speed; // how many frames per one envelope tick
} Instrument;

typedef struct {
    Instrument instrument;
    Note note;
} Sound;

extern Palette PALETTE_DEFAULT;

typedef const FontDefinition *Font;

extern const Font FONT_TINY;
extern const Font FONT_SMALL;
extern const Font FONT_MEDIUM;
extern const Font FONT_LARGE;

typedef uint8_t *SpriteSheet;
typedef uint8_t *TileMap;
typedef Sound *SoundLib;


void sig8_Initialize(const char *windowName);

#define SIG8_PRE_INIT_EDITORS
#define SIG8_PRE_INIT_RESOURCES

// Enable editors
#ifdef SIG8_USE_EDITORS
#undef SIG8_PRE_INIT_EDITORS
#define SIG8_PRE_INIT_EDITORS EnableEditors();
#endif

// Load resources from directory
#ifdef SIG8_USE_RESOURCE_PATH
#undef SIG8_PRE_INIT_RESOURCES
#define SIG8_PRE_INIT_RESOURCES UseResourcePath(SIG8_USE_RESOURCE_PATH);
#endif

// Automatically load resource bundles
#ifdef SIG8_USE_DEFAULT_BUNDLE
extern const uint8_t *SIG8_RESOURCE_BUNDLE;
#undef SIG8_PRE_INIT_RESOURCES
#define SIG8_PRE_INIT_RESOURCES UseResourceBundle(SIG8_RESOURCE_BUNDLE);
#endif

#define SIG8_PRE_INIT SIG8_PRE_INIT_EDITORS SIG8_PRE_INIT_RESOURCES
#define Initialize SIG8_PRE_INIT sig8_Initialize

void Finalize(void);
void Quit(void);
bool ShouldQuit(void);
void SetCursorShape(CursorShape cursor);
int GetScreenWidth(void);
int GetScreenHeight(void);
void RunMainLoop(void (*function)(void));
void SetVSyncEnabled(bool enabled);
void ResizeScreen(int newWidth, int newHeight);
void UsePalette(Palette palette);
Palette GetPalette(void);
int GetBestColor(int r, int g, int b);

/*
 * Resource / filesystem functions.
 * Mostly for internal use.
 */
void UseResourceBundle(const uint8_t *bundle);
void UseResourcePath(const char *path);
uint8_t *ReadFileContents(const char *path, int *size);
void WriteFileContents(const char *path, const void *data, int size);
const uint8_t *GetResourceBundle(void);
const char *GetResourcePath(void);
char *ResolvePath(const char *path);

/*
 * Editors. These functions do nothing when
 * support for editors is disabled.
 */
void EnableEditors(void);
void EditResource(void *resource);

/*
 * Utility functions
 */
Color ColorFromHex(const char *hex);
Color ColorFromIndex(int color);
void* TempAlloc(int n);
char *Format(const char *fmt, ...);
float GetDelta(void);
int Modulo(int a, int b);
int Divide(int a, int b);
bool AnyEventsHappened(void);

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
int GetPixel(int x, int y);
void UseFont(Font font);
void DrawString(int x, int y, int color, const char *string);
int MeasureString(const char *string);
void StrokeRect(int x, int y, int w, int h, int color);
void FillRect(int x, int y, int w, int h, int color);
void DrawLine(int x0, int y0, int x1, int y1, int color);

void UseSpriteSheet(SpriteSheet spriteSheet);
void FreeSpriteSheet(SpriteSheet spriteSheet);
SpriteSheet GetCurrentSpriteSheet(void);
void DrawSprite(int x, int y, int sprite);
void DrawSpriteMask(int x, int y, int sprite, int mask);
void DrawBigSprite(int x, int y, int sprite, int w, int h);
void DrawBigSpriteMask(int x, int y, int sprite, int w, int h, int mask);
void DrawSubSprite(int x, int y, int sprite, int sx, int sy, int w, int h, int mask);
int GetSpritePixel(int x, int y, int sprite);
void SetSpritePixel(int x, int y, int sprite, int color);
SpriteSheet LoadSpriteSheet(const char *path);

/*
 * TileMap functions
 */

TileMap NewTileMap(void);
TileMap LoadTileMap(const char *path);
void FreeTileMap(TileMap tileMap);
void UseTileMap(TileMap tileMap);
TileMap GetCurrentTileMap(void);
void SetTile(int x, int y, int tile);
int GetTile(int x, int y);

typedef void (*TileMapDrawCallback)(int x, int y, int *sprite, int *mask);
void DrawTileMap(int x, int y, int width, int height, int offsetX, int offsetY);
void DrawTileMapEx(int x, int y, int width, int height, int offsetX, int offsetY, int mask, TileMapDrawCallback callback);

/*
 * Audio functions
 */

Instrument NewInstrument(void);
void UseInstrument(Instrument instrument, int channel);
void PlayNote(Note note, int channel);
void StopNote(int channel);
void SetMasterVolume(float volume);
void SetChannelVolume(int channel, float volume);

SoundLib LoadSoundLib(const char *path);
SoundLib GetCurrentSoundLib(void);
void FreeSoundLib(SoundLib soundLib);
void UseSoundLib(SoundLib soundLib);
void PlaySound(int sound, int channel);

#ifdef  __cplusplus
};
#endif
