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
static int width, height;

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

static Position neighbors[4] = {
        { .x = 1, .y = 0 },
        { .x = -1, .y = 0 },
        { .x = 0, .y = 1 },
        { .x = 0, .y = -1 },
};

static void Fill(int x, int y, int color)
{
    Position *queue = malloc(width * height * sizeof(Position));
    uint8_t *used = calloc(width * height, 1);
    int front = 0;
    int back = 0;

    queue[front++] = (Position){
        .x = x,
        .y = y,
    };
    used[x + y * width] = 1;

    while (front > back) {
        Position pos = queue[back++];
        for (int i = 0; i < 4; ++i) {
            Position next = {
                    .x = pos.x + neighbors[i].x,
                    .y = pos.y + neighbors[i].y
            };

            if (next.x < 0 || next.y < 0 || next.x >= width || next.y >= height) {
                continue;
            }

            if (used[next.x + next.y * width]) {
                continue;
            }

            if (GetSpritePixel(next.x, next.y, selected) !=
                GetSpritePixel(pos.x, pos.y, selected)) {
                continue;
            }

            queue[front++] = next;
            used[next.x + next.y * width] = 1;
        }
    }

    for (int j = 0; j < height; ++j) {
        for (int i = 0; i < width; ++i) {
            if (used[i + j * width]) {
                SetSpritePixel(i, j, selected, color);
            }
        }
    }

    free(queue);
    free(used);
}

static void UseTool(Tool tool)
{
    UseSpriteSheet(editing);

    if (tool == ERASE) {
        for (int j = 0; j < height; ++j) {
            for (int i = 0; i < width; ++i) {
                SetSpritePixel(i, j, selected, bgColor);
            }
        }
    } else if (tool == FLIP_H || tool == FLIP_V || tool == ROTATE) {
        uint8_t *data = malloc(width * height);
        uint8_t *ptr = data;

        for (int j = 0; j < height; ++j) {
            for (int i = 0; i < width; ++i) {
                *ptr++ = GetSpritePixel(i, j, selected);
            }
        }

        ptr = data;

        for (int j = 0; j < height; ++j) {
            for (int i = 0; i < width; ++i) {
                int color = *ptr++;

                if (tool == FLIP_H) {
                    SetSpritePixel(width - 1 - i, j, selected, color);
                }

                if (tool == FLIP_V) {
                    SetSpritePixel(i, height - 1 - j, selected, color);
                }

                if (tool == ROTATE) {
                    SetSpritePixel(j, height - 1 - i, selected, color);
                }
            }
        }

        free(data);
    } else {
        activeTool = tool;
    }

    UseSpriteSheet(MAIN_SPRITESHEET);
}

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

    for (int j = 0; j < height; ++j) {
        for (int i = 0; i < width; ++i) {
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
                    if (activeTool == BRUSH) {
                        SetSpritePixel(i, j, selected, fgColor);
                    }

                    if (activeTool == FILL) {
                        Fill(i, j, fgColor);
                    }

                    if (activeTool == EYEDROPPER) {
                        fgColor = GetSpritePixel(i, j, selected);
                    }
                }

                if (MousePressed(MOUSE_RIGHT)) {
                    if (activeTool == BRUSH) {
                        SetSpritePixel(i, j, selected, bgColor);
                    }

                    if (activeTool == FILL) {
                        Fill(i, j, bgColor);
                    }

                    if (activeTool == EYEDROPPER) {
                        bgColor = GetSpritePixel(i, j, selected);
                    }
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

    for (int j = 0; j < SPR_Y; ++j) {
        for (int i = 0; i < SPR_X; ++i) {
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
                .width = width,
                .height = height
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
    width = SPRITE_WIDTH * spriteRegion;
    height = SPRITE_HEIGHT * spriteRegion;
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
                UseTool(tool);
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
    width = SPRITE_WIDTH;
    height = SPRITE_HEIGHT;
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
