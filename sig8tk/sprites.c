#include "sig8tk.h"

#define SPR_X 16
#define SPR_Y 16
#define EDIT_X 64
#define EDIT_Y 64
#define PALETTE_STRIDE 7
#define ROW_COLORS 8

static SpriteSheet editing;
static int editingIndex;
static int selected, selectedX, selectedY;
static int fgColor = WHITE;
static int bgColor = BLACK;
static int zoom = 8;
static int spriteRegion, spriteRegionLog;

typedef enum {
    BRUSH,
    FILL,
    SELECT,
    EYEDROPPER,
    FLIP_H,
    FLIP_V,
    ROTATE,
    ERASE,

    NUMBER_OF_TOOLS
} Tool;

static Tool activeTool;

static void FixSelected()
{
    if (selectedX < 0) selectedX = 0;
    if (selectedY < 0) selectedY = 0;
    if (selectedX + spriteRegion > SPR_X) selectedX = SPR_X - spriteRegion;
    if (selectedY + spriteRegion > SPR_Y) selectedY = SPR_Y - spriteRegion;

    selected = selectedX + selectedY * SPR_X;
}

static void DrawPalette(void)
{
    Rect rect = {
            .x = 4,
            .y = TOOLBAR_SIZE + 75,
            .width = ROW_COLORS * (PALETTE_STRIDE + 1) + 1,
            .height = (PALETTE_SIZE / ROW_COLORS) * (PALETTE_STRIDE + 1) + 1
    };

    for (int color = 0; color < PALETTE_SIZE; ++color) {
        int i = color % ROW_COLORS;
        int j = color / ROW_COLORS;

        Rect r = {
                .x = rect.x + i * (PALETTE_STRIDE + 1),
                .y = rect.y + j * (PALETTE_STRIDE + 1),
                .width = PALETTE_STRIDE,
                .height = PALETTE_STRIDE
        };

        FillRectR(r, color);
        StrokeRectR(AddBorder(r, 1), WHITE);

        if (IsMouseOver(r)) {
            SetCursorShape(CURSOR_HAND);

            if (MousePressed(MOUSE_LEFT)) {
                fgColor = color;
            }

            if (MousePressed(MOUSE_RIGHT)) {
                bgColor = color;
            }
        }
    }

    {
        // draw fg color
        int i = fgColor % 8;
        int j = fgColor / 8;

        Rect r = {
                .x = rect.x + i * (PALETTE_STRIDE + 1),
                .y = rect.y + j * (PALETTE_STRIDE + 1),
                .width = PALETTE_STRIDE,
                .height = PALETTE_STRIDE
        };

        FillRectR(AddBorder(r, 1), fgColor);
        StrokeRectR(AddBorder(r, 2), WHITE);
    }

    {
        // draw bg color
        int i = bgColor % 8;
        int j = bgColor / 8;

        Rect r = {
                .x = rect.x + i * (PALETTE_STRIDE + 1),
                .y = rect.y + j * (PALETTE_STRIDE + 1),
                .width = PALETTE_STRIDE,
                .height = PALETTE_STRIDE
        };

        if (bgColor == WHITE) {
            RemapColor(WHITE, BLACK);
        }

        DrawSprite(r.x, r.y, 4, SPRITE_MASK_COLOR(BLACK));
        ResetColors();
    }
}

static void DrawEditedSprite(void)
{
    Rect rect = {
            .x = 4,
            .y = TOOLBAR_SIZE + 4,
            .width = EDIT_X,
            .height = EDIT_Y
    };

    StrokeRectR(AddBorder(rect, 1), WHITE);

    UseSpriteSheet(editing);

    int selX = -1;
    int selY = -1;

    for (int i = 0; i < SPRITE_WIDTH * spriteRegion; ++i) {
        for (int j = 0; j < SPRITE_HEIGHT * spriteRegion; ++j) {
            Rect r = {
                    .x = rect.x + i * zoom,
                    .y = rect.y + j * zoom,
                    .width = zoom,
                    .height = zoom
            };

            int color = GetSpritePixel(i, j, selected);
            FillRectR(r, color);

            if (IsMouseOver(r)) {
                SetCursorShape(CURSOR_HAND);
                selX = i;
                selY = j;

                if (MousePressed(MOUSE_LEFT)) {
                    SetSpritePixel(i, j, selected, fgColor);
                }

                if (MousePressed(MOUSE_RIGHT)) {
                    SetSpritePixel(i, j, selected, bgColor);
                }
            }
        }
    }

    if (selX != -1) {
        Rect r = {
                .x = rect.x + selX * zoom,
                .y = rect.y + selY * zoom,
                .width = zoom - 1,
                .height = zoom - 1
        };
        StrokeRectR(AddBorder(r, 1), BLACK);
        StrokeRectR(AddBorder(r, 2), WHITE);
    }

    UseSpriteSheet(MAIN_SPRITESHEET);
}

static void DrawSpriteSheet(void)
{
    Rect rect = {
            .y = TOOLBAR_SIZE,
            .width = SPR_X * SPRITE_WIDTH,
            .height = SPR_Y * SPRITE_HEIGHT,
    };
    rect.x = SCREEN_WIDTH - rect.width;

    UseSpriteSheet(editing);

    for (int i = 0; i < SPR_X; ++i) {
        for (int j = 0; j < SPR_Y; ++j) {
            int idx = i + j * SPR_X;
            Rect r = {
                    .x = rect.x + i * SPRITE_WIDTH,
                    .y = rect.y + j * SPRITE_HEIGHT,
                    .width = SPRITE_WIDTH,
                    .height = SPRITE_HEIGHT
            };
            DrawSprite(r.x, r.y, idx, 0);

            if (IsMouseOver(r)) {
                SetCursorShape(CURSOR_HAND);
                if (MousePressed(MOUSE_LEFT)) {
                    selectedX = i - spriteRegion / 2;
                    selectedY = j - spriteRegion / 2;
                    FixSelected();
                }
            }
        }
    }

    UseSpriteSheet(MAIN_SPRITESHEET);

    {
        int i = selected % SPR_X;
        int j = selected / SPR_X;

        Rect r = {
                .x = rect.x + i * SPRITE_WIDTH,
                .y = rect.y + j * SPRITE_HEIGHT,
                .width = SPRITE_WIDTH * spriteRegion,
                .height = SPRITE_HEIGHT * spriteRegion
        };
        StrokeRectR(AddBorder(r, 2), WHITE);
        StrokeRectR(AddBorder(r, 1), BLACK);
    }
}

static void DrawStatusBar(void)
{
    char *string = Format("#%03d", selected);
    DrawString(SCREEN_WIDTH - 23, SCREEN_HEIGHT, RED, string);

    DrawSlider(SCREEN_WIDTH - 60, SCREEN_HEIGHT - 7, &spriteRegionLog);
    spriteRegion = 1 << spriteRegionLog;
    zoom = 8 / spriteRegion;
    FixSelected();
}

static void DrawTools(void)
{
    Rect rect = {
            .x = 4,
            .y = TOOLBAR_SIZE + 95
    };

    for (int tool = 0; tool < NUMBER_OF_TOOLS; ++tool) {
        Rect r = {
            .x = rect.x + SPRITE_WIDTH * tool,
            .y = rect.y,
            .width = 7,
            .height = 7
        };

        if (tool == activeTool) {
            DrawIcon(r.x, r.y + 1, 5 + tool, WHITE);
        } else {
            DrawIcon(r.x, r.y + 1, 5 + tool, BLACK);
            DrawIcon(r.x, r.y, 5 + tool, GRAY);
        }

        if (IsMouseOver(r)) {
            SetCursorShape(CURSOR_HAND);
            if (MouseJustPressed(MOUSE_LEFT)) {
                activeTool = tool;
            }
        }
    }
    ResetColors();
}

void SpritesInit(void)
{
    editing = MAIN_SPRITESHEET;
    selected = 0;
    editingIndex = 0;
    spriteRegion = 1;
    zoom = 8;
    activeTool = BRUSH;
}

void SpritesTick(void)
{
    ClearScreen(INDIGO);
    DrawToolbar();
    DrawSpriteSheet();
    DrawEditedSprite();
    DrawPalette();
    DrawStatusBar();
    DrawTools();
    DrawNumberInput(SCREEN_WIDTH - 18, 1, &editingIndex);
}
