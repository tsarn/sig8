#pragma once

#include <stdint.h>
#include <stddef.h>

#ifdef  __cplusplus
extern "C" {
#endif

typedef struct {
    uint8_t r, g, b, a;
} Color;

typedef struct {
    uint8_t firstCharCode;
    uint8_t lastCharCode;
    uint8_t isMono;
    uint8_t width;
    uint8_t height;
    uint8_t horizontalStep;
    uint8_t verticalStep;
    const uint8_t *fontData;
} Font;

extern const Font fontMono5x7;

// System functions
void Initialize(void);
void Finalize(void);
int ShouldQuit(void);

// Utility functions
Color ColorFromHex(const char *hex);
int GetScreenWidth(void);
int GetScreenHeight(void);
void* TempAlloc(size_t n);
char *Format(const char *fmt, ...);

// Time functions
float GetDelta(void);

// Drawing functions
void ClearScreen(int color);
void DrawPixel(int x, int y, int color);
void SetFont(const Font* font);
void DrawCharacter(int x, int y, int color, char ch);
void DrawString(int x, int y, int color, const char *string);

#ifdef  __cplusplus
};
#endif
