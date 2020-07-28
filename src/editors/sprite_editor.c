#include "editors.h"

#define EDIT_X 64
#define EDIT_Y 64
#define PALETTE_STRIDE 7
#define ROW_COLORS 8

#define TOOLBAR_SIZE 10

static sig8_ManagedResource *editing;
static int selected;
static int fgColor = WHITE;
static int bgColor = BLACK;
static int zoom = 8;
static int spriteRegion = 1, spriteRegionLog;
static int width = SPRITE_WIDTH, height = SPRITE_HEIGHT;
static int brushSize;
static HistoryItem curAction;
static History history;

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

static const Button toolButtons[] = {
    { NULL, "BRUSH [B]", "B", 5 },
    { NULL, "FILL [F]", "F", 6 },
    { NULL, "SELECT [S]", "S", 7 },
    { NULL, "COLOR PICKER [P]", "P", 8 },
    { NULL, "FLIP HORIZONTALLY [H]", "H", 9 },
    { NULL, "FLIP VERTICALLY [V]", "V", 10 },
    { NULL, "ROTATE [R]", "R", 11 },
    { NULL, "CLEAR SPRITE [DEL]", "Delete", 12 }
};

static Tool activeTool = BRUSH;

static void BeginUndoableAction(void)
{
    curAction.data = TempAlloc(SPRITE_SHEET_BYTE_SIZE);
    memcpy(curAction.data, editing->resource, SPRITE_SHEET_BYTE_SIZE);
}

static void EndUndoableAction(void)
{
    bool anythingChanged = false;
    for (int i = 0; i < SPRITE_SHEET_BYTE_SIZE; ++i) {
        curAction.data[i] ^= editing->resource[i];
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
            editing->resource[j] ^= t;
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

static void Save(void)
{
    if (!editing->path) {
        return;
    }

    char *path = ResolvePath(editing->path);

    if (!path) {
        return;
    }

    uint8_t data[3 * SPRITE_WIDTH * SPRITE_HEIGHT * SPRITE_SHEET_SIZE];
    for (int i = 0; i < SPRITE_WIDTH * SPRITE_HEIGHT * SPRITE_SHEET_SIZE; ++i) {
        Color color = ColorFromIndex(editing->resource[i]);
        data[3 * i] = color.r;
        data[3 * i + 1] = color.g;
        data[3 * i + 2] = color.b;
    }

    stbi_write_png(path, SPR_X * SPRITE_WIDTH, SPR_Y * SPRITE_HEIGHT, 3, data, 0);

    free(path);
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
    UseSpriteSheet(editing->resource);

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

    UseSpriteSheet(editing->resource);

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

                int *col = &fgColor;

                if (MousePressed(MOUSE_LEFT) || MousePressed(MOUSE_RIGHT)) {
                    if (MousePressed(MOUSE_RIGHT)) {
                        col = &bgColor;
                    }

                    BeginUndoableAction();
                    if (activeTool == BRUSH) {
                        for (int x = selX; (x < selX + br) && (x < width); ++x) {
                            for (int y = selY; (y < selY + br) && (y < height); ++y) {
                                SetSpritePixel(x, y, selected, *col);
                            }
                        }
                    }

                    if (activeTool == FILL) {
                        Fill(i, j, *col);
                    }

                    if (activeTool == COLOR_PICKER) {
                        *col = GetSpritePixel(i, j, selected);
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
    sig8_DrawSpriteSheet(
            SCREEN_WIDTH - SPR_X * SPRITE_WIDTH, TOOLBAR_SIZE,
            editing->resource, spriteRegion, &selected
    );
}

static void DrawTopButtons(void)
{
    static const Button UndoButton = {
            .sprite = 13,
            .shortcut = "Ctrl+Z",
            .hint = "UNDO [CTRL-Z]",
            .handler = Undo
    };

    static const Button RedoButton = {
            .sprite = 14,
            .shortcut = "Ctrl+Y",
            .hint = "REDO [CTRL-Y]",
            .handler = Redo
    };

    static const Button SaveButton = {
            .sprite = 15,
            .shortcut = "Ctrl+S",
            .hint = "SAVE [CTRL-S]",
            .handler = Save
    };

    sig8_DrawButton(SCREEN_WIDTH - 25, 1, UndoButton, false);
    sig8_DrawButton(SCREEN_WIDTH - 17, 1, RedoButton, false);
    sig8_DrawButton(SCREEN_WIDTH - 9, 1, SaveButton, false);
}

static void DrawStatusBar(void)
{
    FillRect(0, 0, SCREEN_WIDTH, TOOLBAR_SIZE, DARK_BLUE);
    DrawTopButtons();
    FillRect(0, SCREEN_HEIGHT - TOOLBAR_SIZE, SCREEN_WIDTH, TOOLBAR_SIZE, DARK_BLUE);

    char *string = Format("#%03d", selected);
    DrawString(SCREEN_WIDTH - 23, SCREEN_HEIGHT - 1, RED, string);
    UseFont(FONT_3X5);
    DrawString(2, SCREEN_HEIGHT - 2, GRAY, sig8_StatusLine);
    UseFont(FONT_ASEPRITE);
    if (editing->path) {
        DrawString(2, 8, GRAY, editing->path);
    }

    if (activeTool == BRUSH) {
        sig8_DrawSlider(4, 116, &brushSize);
    }

    sig8_DrawSlider(SCREEN_WIDTH - 60, SCREEN_HEIGHT - 7, &spriteRegionLog);
    spriteRegion = 1 << spriteRegionLog;
    zoom = 8 / spriteRegion;
    width = SPRITE_WIDTH * spriteRegion;
    height = SPRITE_HEIGHT * spriteRegion;
}

static void DrawTools(void)
{
    for (int tool = 0; tool < NUMBER_OF_TOOLS; ++tool) {
        if (sig8_DrawButton(4 + SPRITE_WIDTH * tool, TOOLBAR_SIZE + 95,
                toolButtons[tool], (Tool)tool == activeTool)) {
            UseTool(tool);
        }
    }
}

static void HandleInput(void)
{
    if (KeyJustPressed("Escape")) {
        SetCursorShape(CURSOR_ARROW);
        sig8_HistoryClear(&history);
        sig8_LeaveEditor();
        return;
    }
}

void sig8_SpriteEditorInit(sig8_ManagedResource *what)
{
    editing = what;
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
    sig8_StatusLine = "";
    DrawTools();
    DrawStatusBar();
    DrawSpriteSheet();
    DrawEditedSprite();
    DrawPalette();
    HandleInput();
}
