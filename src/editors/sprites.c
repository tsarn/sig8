#ifdef SIG8_COMPILE_EDITORS
#include "editors.h"

#define SPR_X 16
#define SPR_Y 16
#define EDIT_X 64
#define EDIT_Y 64
#define PALETTE_STRIDE 7
#define ROW_COLORS 8

static SpriteSheet editing;
static int selected, selectedX, selectedY;
static int fgColor = WHITE;
static int bgColor = BLACK;
static int zoom = 8;
static int spriteRegion, spriteRegionLog;
static int width, height;
static int brushSize;
static HistoryItem curAction;
static History history;
static const char *statusLine;

typedef enum {
    BRUSH,
    FILL,
    SELECT,
    COLOR_PICKER,
    FLIP_H,
    FLIP_V,
    ROTATE,
    ERASE,

    NUMBER_OF_TOOLS
} Tool;

static const char* toolNames[] = {
    "BRUSH",
    "FILL",
    "SELECT",
    "COLOR PICKER",
    "FLIP HORIZONTALLY",
    "FLIP VERTICALLY",
    "ROTATE",
    "CLEAR SPRITE",
};

static Tool activeTool;

static void BeginUndoableAction(void)
{
    curAction.data = TempAlloc(SPRITE_SHEET_BYTE_SIZE);
    memcpy(curAction.data, editing, SPRITE_SHEET_BYTE_SIZE);
}

static void EndUndoableAction(void)
{
    bool anythingChanged = false;
    for (int i = 0; i < SPRITE_SHEET_BYTE_SIZE; ++i) {
        curAction.data[i] ^= editing[i];
        if (curAction.data[i]) {
            anythingChanged = true;
        }
    }

    if (!anythingChanged) {
        return;
    }

    // very simple compression
    int last = curAction.data[0], len = 1, j = 0;
    for (int i = 1; i <= SPRITE_SHEET_BYTE_SIZE; ++i) {
        if (i < SPRITE_SHEET_BYTE_SIZE) {
            if (curAction.data[i] == last) {
                if (len < 255) {
                    ++len;
                    continue;
                }
            }
        }

        curAction.data[j++] = len;
        curAction.data[j++] = last;

        last = curAction.data[i];
        len = 1;
    }

    uint8_t *oldData = curAction.data;

    curAction.data = malloc(j);
    memcpy(curAction.data, oldData, j);
    sig8_HistoryPush(&history, curAction);
    curAction.data = NULL;
}

static void ApplyUndo(HistoryItem historyItem)
{
    int i = 0, j = 0;
    while (j < SPRITE_SHEET_BYTE_SIZE) {
        int end = historyItem.data[i++] + j;
        uint8_t t = historyItem.data[i++];
        for (; j < end; ++j) {
            editing[j] ^= t;
        }
    }
}

static void Undo(void)
{
    if (sig8_HistoryCanUndo(&history)) {
        ApplyUndo(sig8_HistoryUndo(&history));
    }
}

static void Redo(void)
{
    if (sig8_HistoryCanRedo(&history)) {
        ApplyUndo(sig8_HistoryRedo(&history));
    }
}

static void Fill(int x, int y, int color)
{
    // simple breadth-first search

    static const Position neighbors[4] = {
            { .x = 1, .y = 0 },
            { .x = -1, .y = 0 },
            { .x = 0, .y = 1 },
            { .x = 0, .y = -1 },
    };

    Position *queue = TempAlloc(width * height * sizeof(Position));
    uint8_t *used = TempAlloc(width * height);
    memset(used, 0, width * height);
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
}

static void UseTool(Tool tool)
{
    UseSpriteSheet(editing);

    if (tool == ERASE) {
        BeginUndoableAction();
        for (int j = 0; j < height; ++j) {
            for (int i = 0; i < width; ++i) {
                SetSpritePixel(i, j, selected, bgColor);
            }
        }
        EndUndoableAction();
    } else if (tool == FLIP_H || tool == FLIP_V || tool == ROTATE) {
        BeginUndoableAction();
        uint8_t *data = TempAlloc(width * height);
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
        EndUndoableAction();
    } else {
        activeTool = tool;
    }

    UseSpriteSheet(sig8_EDITORS_SPRITESHEET);
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

        sig8_FillRectR(r, color);
        sig8_StrokeRectR(sig8_AddBorder(r, 1), WHITE);

        if (sig8_IsMouseOver(r)) {
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

        sig8_FillRectR(sig8_AddBorder(r, 1), fgColor);
        sig8_StrokeRectR(sig8_AddBorder(r, 2), WHITE);
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

    sig8_StrokeRectR(sig8_AddBorder(rect, 1), WHITE);

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
            sig8_FillRectR(r, color);

            if (sig8_IsMouseOver(r)) {
                SetCursorShape(CURSOR_HAND);
                int br = 1;
                if (activeTool == BRUSH) {
                    br = 1 + brushSize;
                }
                selX = i - br / 2;
                selY = j - br / 2;

                if (selX < 0) selX = 0;
                if (selY < 0) selY = 0;
                if (selX + br > width) selX = width - br;
                if (selY + br > height) selY = height - br;

                if (MousePressed(MOUSE_LEFT)) {
                    BeginUndoableAction();
                    if (activeTool == BRUSH) {
                        for (int x = selX; x < selX + br; ++x) {
                            for (int y = selY; y < selY + br; ++y) {
                                SetSpritePixel(x, y, selected, fgColor);
                            }
                        }
                    }

                    if (activeTool == FILL) {
                        Fill(i, j, fgColor);
                    }

                    if (activeTool == COLOR_PICKER) {
                        fgColor = GetSpritePixel(i, j, selected);
                    }
                    EndUndoableAction();
                }

                if (MousePressed(MOUSE_RIGHT)) {
                    BeginUndoableAction();
                    if (activeTool == BRUSH) {
                        SetSpritePixel(i, j, selected, bgColor);
                    }

                    if (activeTool == FILL) {
                        Fill(i, j, bgColor);
                    }

                    if (activeTool == COLOR_PICKER) {
                        bgColor = GetSpritePixel(i, j, selected);
                    }
                    EndUndoableAction();
                }
            }
        }
    }

    if (selX != -1) {
        int br = 1;
        if (activeTool == BRUSH) {
            br = 1 + brushSize;
        }

        Rect r = {
                .x = rect.x + selX * zoom,
                .y = rect.y + selY * zoom,
                .width = zoom * br,
                .height = zoom * br
        };
        sig8_StrokeRectR(sig8_AddBorder(r, 1), BLACK);
        sig8_StrokeRectR(sig8_AddBorder(r, 2), WHITE);
    }

    UseSpriteSheet(sig8_EDITORS_SPRITESHEET);
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

            if (sig8_IsMouseOver(r)) {
                SetCursorShape(CURSOR_HAND);
                if (MousePressed(MOUSE_LEFT)) {
                    selectedX = i - spriteRegion / 2;
                    selectedY = j - spriteRegion / 2;
                    FixSelected();
                }
            }
        }
    }

    UseSpriteSheet(sig8_EDITORS_SPRITESHEET);

    {
        int i = selected % SPR_X;
        int j = selected / SPR_X;

        Rect r = {
                .x = rect.x + i * SPRITE_WIDTH,
                .y = rect.y + j * SPRITE_HEIGHT,
                .width = width,
                .height = height
        };
        sig8_StrokeRectR(sig8_AddBorder(r, 2), WHITE);
        sig8_StrokeRectR(sig8_AddBorder(r, 1), BLACK);
    }
}

static void DrawStatusBar(void)
{
    char *string = Format("#%03d", selected);
    DrawString(SCREEN_WIDTH - 23, SCREEN_HEIGHT, RED, string);
    SetFont(FONT_3X5);
    DrawString(2, SCREEN_HEIGHT - 2, GRAY, statusLine);
    SetFont(FONT_ASEPRITE);

    if (activeTool == BRUSH) {
        sig8_DrawSlider(4, 116, &brushSize);
    }

    sig8_DrawSlider(SCREEN_WIDTH - 60, SCREEN_HEIGHT - 7, &spriteRegionLog);
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

        if ((Tool)tool == activeTool) {
            sig8_DrawIcon(r.x, r.y + 1, 5 + tool, WHITE);
        } else {
            sig8_DrawIcon(r.x, r.y + 1, 5 + tool, BLACK);
            sig8_DrawIcon(r.x, r.y, 5 + tool, GRAY);
        }

        if (sig8_IsMouseOver(r)) {
            SetCursorShape(CURSOR_HAND);
            statusLine = toolNames[tool];
            if (MouseJustPressed(MOUSE_LEFT)) {
                UseTool(tool);
            }
        }
    }
    ResetColors();
}

static void HandleInput(void)
{
    if (KeyJustPressed("Ctrl+Z")) {
        Undo();
    }

    if (KeyJustPressed("Ctrl+Y")) {
        Redo();
    }
}

void sig8_SpriteEditorInit(sig8_ManagedResource *what)
{
    editing = what->resource;
    selected = 0;
    spriteRegion = 1;
    brushSize = 0;
    zoom = 8;
    width = SPRITE_WIDTH;
    height = SPRITE_HEIGHT;
    activeTool = BRUSH;
    history = (History) {
        .size = 0,
        .capacity = 0,
        .cur = 0,
        .data = NULL
    };
}

void sig8_SpriteEditorTick(void)
{
    UseSpriteSheet(sig8_EDITORS_SPRITESHEET);
    SetCursorShape(CURSOR_ARROW);

    ClearScreen(INDIGO);
    statusLine = "";
    sig8_DrawToolbar();
    DrawSpriteSheet();
    DrawEditedSprite();
    DrawPalette();
    DrawTools();
    DrawStatusBar();
    HandleInput();

    if (KeyJustPressed("Escape")) {
        SetCursorShape(CURSOR_ARROW);
        sig8_HistoryClear(&history);
        sig8_LeaveEditor();
    }
}

#endif
