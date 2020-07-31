#include "editors.h"

#define EDIT_X 64
#define EDIT_Y 64
#define PALETTE_STRIDE 7
#define ROW_COLORS 8

#define TOOLBAR_SIZE 10

static int selected;
static int fgColor;
static int bgColor;
static int zoom = 8;
static int spriteRegion = 1, spriteRegionLog;
static int width = SPRITE_WIDTH, height = SPRITE_HEIGHT;
static int brushSize;

static Palette palette;
static int black, white;

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
        {"BRUSH [B]",             "B",      5},
        {"FILL [F]",              "F",      6},
        {"SELECT [S]",            "S",      7},
        {"COLOR PICKER [P]",      "P",      8},
        {"FLIP HORIZONTALLY [H]", "H",      9},
        {"FLIP VERTICALLY [V]",   "V",      10},
        {"ROTATE [R]",            "R",      11},
        {"CLEAR SPRITE [DEL]",    "Delete", 12}
};

static Tool activeTool = BRUSH;

static void Save(void)
{
    if (!sig8_Editing->path) {
        return;
    }

    char *path = ResolvePath(sig8_Editing->path);

    if (!path) {
        return;
    }

    UsePalette(palette);

    uint8_t data[3 * SPRITE_WIDTH * SPRITE_HEIGHT * SPRITESHEET_SIZE];
    for (int i = 0; i < SPRITE_WIDTH * SPRITE_HEIGHT * SPRITESHEET_SIZE; ++i) {
        Color color = ColorFromIndex(sig8_Editing->resource[i]);
        data[3 * i] = color.r;
        data[3 * i + 1] = color.g;
        data[3 * i + 2] = color.b;
    }

    UsePalette(PALETTE_DEFAULT);

    stbi_write_png(path, SPRITESHEET_WIDTH * SPRITE_WIDTH, SPRITESHEET_HEIGHT * SPRITE_HEIGHT, 3, data, 0);

    free(path);
}

static void Fill(int x, int y, int color)
{
    static const Position neighbors[4] = {
            {.x = 1, .y = 0},
            {.x = -1, .y = 0},
            {.x = 0, .y = 1},
            {.x = 0, .y = -1},
    };

    Position *queue = TempAlloc(width * height * sizeof(Position));
    uint8_t *used = TempAlloc(width * height);
    memset(used, 0, width * height);
    int front = 0;
    int back = 0;

    queue[front++] = (Position) {
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
    UseSpriteSheet(sig8_Editing->resource);

    if (tool == ERASE) {
        sig8_BeginUndoableAction();

        for (int j = 0; j < height; ++j) {
            for (int i = 0; i < width; ++i) {
                SetSpritePixel(i, j, selected, bgColor);
            }
        }

        sig8_EndUndoableAction();
    } else if (tool == FLIP_H || tool == FLIP_V || tool == ROTATE) {
        sig8_BeginUndoableAction();
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

        sig8_EndUndoableAction();
    } else {
        activeTool = tool;
    }

    UseSpriteSheet(sig8_EDITORS_SPRITESHEET);
}

static void DrawPalette(void)
{
    UsePalette(palette);

    Rect rect = {
            .x = 3,
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
        sig8_StrokeRectR(sig8_AddBorder(r, 1), white);

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
        sig8_StrokeRectR(sig8_AddBorder(r, 2), white);
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

        if (bgColor == white) {
            RemapColor(WHITE, black);
        } else {
            RemapColor(WHITE, white);
        }

        DrawSprite(r.x, r.y, 4);
        ResetColors();
    }

    UsePalette(PALETTE_DEFAULT);
}

static void DrawEditedSprite(void)
{
    Rect rect = {
            .x = 3,
            .y = TOOLBAR_SIZE + 4,
            .width = EDIT_X,
            .height = EDIT_Y
    };


    UsePalette(palette);
    sig8_StrokeRectR(sig8_AddBorder(rect, 1), white);

    UseSpriteSheet(sig8_Editing->resource);

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

                    sig8_BeginUndoableAction();

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

                    sig8_EndUndoableAction();
                }
            }
        }
    }

    UseSpriteSheet(sig8_EDITORS_SPRITESHEET);

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
        sig8_StrokeRectR(sig8_AddBorder(r, 1), black);
        sig8_StrokeRectR(sig8_AddBorder(r, 2), white);
    }

    UsePalette(PALETTE_DEFAULT);
}

static void DrawSpriteSheet(void)
{
    UsePalette(palette);
    sig8_DrawSpriteSheet(
            SCREEN_WIDTH - SPRITESHEET_WIDTH * SPRITE_WIDTH - 1, TOOLBAR_SIZE + 1,
            sig8_Editing->resource, spriteRegion, &selected
    );
    UsePalette(PALETTE_DEFAULT);
}

static void DrawTopButtons(void)
{
    if (sig8_DrawButton(SCREEN_WIDTH - 25, 1, (Button) {
            .sprite = 13,
            .shortcut = "Ctrl+Z",
            .hint = "UNDO [CTRL-Z]"
    }, false)) {
        sig8_Undo();
    }

    if (sig8_DrawButton(SCREEN_WIDTH - 17, 1, (Button) {
            .sprite = 14,
            .shortcut = "Ctrl+Y",
            .hint = "REDO [CTRL-Y]"
    }, false)) {
        sig8_Redo();
    }

    if (sig8_DrawButton(SCREEN_WIDTH - 9, 1, (Button) {
            .sprite = 15,
            .shortcut = "Ctrl+S",
            .hint = "SAVE [CTRL-S]"
    }, false)) {
        Save();
    }
}

static void DrawStatusBar(void)
{
    FillRect(0, 0, SCREEN_WIDTH, TOOLBAR_SIZE, DARK_BLUE);
    DrawTopButtons();
    FillRect(0, SCREEN_HEIGHT - TOOLBAR_SIZE, SCREEN_WIDTH, TOOLBAR_SIZE, DARK_BLUE);

    char *string = Format("#%03d", selected);
    UseFont(FONT_SMALL);
    DrawString(SCREEN_WIDTH - 23, SCREEN_HEIGHT - 2, RED, string);
    DrawString(2, SCREEN_HEIGHT - 2, GRAY, sig8_StatusLine);

    if (activeTool == BRUSH) {
        sig8_DrawSlider(4, SCREEN_HEIGHT - 32, &brushSize);
    }

    sig8_DrawSlider(SCREEN_WIDTH - 60, SCREEN_HEIGHT - 8, &spriteRegionLog);
    spriteRegion = 1 << spriteRegionLog;
    zoom = 8 / spriteRegion;
    width = SPRITE_WIDTH * spriteRegion;
    height = SPRITE_HEIGHT * spriteRegion;
}

static void DrawTools(void)
{
    for (int tool = 0; tool < NUMBER_OF_TOOLS; ++tool) {
        if (sig8_DrawButton(3 + SPRITE_WIDTH * tool, TOOLBAR_SIZE + 120,
                            toolButtons[tool], (Tool) tool == activeTool)) {
            UseTool(tool);
        }
    }
}

static void HandleInput(void)
{
    if (KeyJustPressed("Escape")) {
        SetCursorShape(CURSOR_ARROW);
        sig8_HistoryClear();
        sig8_LeaveEditor();
        UsePalette(palette);
        return;
    }
}

void sig8_SpriteEditorInit(ManagedResource *what)
{
    sig8_Editing = what;
    palette = GetPalette();

    black = GetBestColor(0, 0, 0);
    white = GetBestColor(255, 255, 255);

    fgColor = white;
    bgColor = black;
    sig8_HistoryClear();
}

void sig8_SpriteEditorTick(void)
{
    UseSpriteSheet(sig8_EDITORS_SPRITESHEET);
    SetCursorShape(CURSOR_ARROW);
    UseFont(FONT_MEDIUM);
    UsePalette(PALETTE_DEFAULT);

    ClearScreen(INDIGO);
    sig8_StatusLine = "";
    DrawTools();
    DrawStatusBar();
    DrawSpriteSheet();
    DrawEditedSprite();
    DrawPalette();
    HandleInput();
}
