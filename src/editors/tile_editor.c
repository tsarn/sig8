#include "editors.h"

#define TOOLBAR_SIZE 10

static SpriteSheet spriteSheet;

static int ticks = 0;

static int cameraX = 0;
static int cameraY = 0;
static int selectedX;
static int selectedY;

static bool isDragging;
static int dragOriginX;
static int dragOriginY;

static bool spriteTabOpen;
static int selectedSprite;

static bool showGrid;

static Selection selection;
static Selection spriteSelection;

typedef enum {
    TOOL_DRAW,
    TOOL_FILL,
    TOOL_SELECT,
} Tool;

static Tool tool = TOOL_DRAW;

static void Save(void)
{
    if (!sig8_Editing->path) {
        return;
    }

    char *path = ResolvePath(sig8_Editing->path);

    if (!path) {
        return;
    }

    FILE *file = fopen(path, "w");

    if (!file) {
        return;
    }

    int size;
    uint8_t *data = stbi_zlib_compress(sig8_Editing->resource, sig8_Editing->size, &size, 8);
    fwrite(data, size, 1, file);

    fclose(file);
    free(data);
    free(path);
}

static void Fill(int x, int y, int tile)
{
    x = Modulo(x, TILEMAP_WIDTH);
    y = Modulo(y, TILEMAP_HEIGHT);

    static const Position neighbors[4] = {
            {.x = 1, .y = 0},
            {.x = -1, .y = 0},
            {.x = 0, .y = 1},
            {.x = 0, .y = -1},
    };

    int x1, y1, x2, y2;
    bool xInv = false;
    bool yInv = false;

    if (selection.active) {
        x1 = Modulo(selection.x1, TILEMAP_WIDTH);
        y1 = Modulo(selection.y1, TILEMAP_HEIGHT);
        x2 = Modulo(selection.x2, TILEMAP_WIDTH);
        y2 = Modulo(selection.y2, TILEMAP_HEIGHT);

        if (x1 > x2) {
            xInv = true;
            swap(x1, x2);
            ++x1;
            --x2;
        }

        if (y1 > y2) {
            yInv = true;
            swap(y1, y2);
            ++y1;
            --y2;
        }
    } else {
        x1 = y1 = 0;
        x2 = TILEMAP_WIDTH;
        y2 = TILEMAP_HEIGHT;
    }

    if ((x < x1 || x > x2) ^ xInv) {
        return;
    }

    if ((y < y1 || y > y2) ^ yInv) {
        return;
    }

    Position *queue = TempAlloc(TILEMAP_BYTE_SIZE * sizeof(Position));
    uint8_t *used = TempAlloc(TILEMAP_BYTE_SIZE);
    memset(used, 0, TILEMAP_BYTE_SIZE);
    int front = 0;
    int back = 0;

    queue[front++] = (Position) {
            .x = x,
            .y = y,
    };
    used[x + y * TILEMAP_WIDTH] = 1;

    while (front > back) {
        Position pos = queue[back++];
        for (int i = 0; i < 4; ++i) {
            Position next = {
                    .x = Modulo(pos.x + neighbors[i].x, TILEMAP_WIDTH),
                    .y = Modulo(pos.y + neighbors[i].y, TILEMAP_HEIGHT)
            };

            if ((next.x < x1 || next.x > x2) ^ xInv) {
                continue;
            }

            if ((next.y < y1 || next.y > y2) ^ yInv) {
                continue;
            }

            if (used[next.x + next.y * TILEMAP_WIDTH]) {
                continue;
            }

            if (GetTile(next.x, next.y) != GetTile(pos.x, pos.y)) {
                continue;
            }

            queue[front++] = next;
            used[next.x + next.y * TILEMAP_WIDTH] = 1;
        }
    }

    for (int j = 0; j < TILEMAP_HEIGHT; ++j) {
        for (int i = 0; i < TILEMAP_WIDTH; ++i) {
            if (used[i + j * TILEMAP_WIDTH]) {
                SetTile(i, j, tile);
            }
        }
    }
}

static void Clear(void)
{
    if (selection.resizing) {
        return;
    }

    sig8_BeginUndoableAction();

    if (selection.active) {
        for (int j = selection.y1; j <= selection.y2; ++j) {
            for (int i = selection.x1; i <= selection.x2; ++i) {
                SetTile(i, j, 0);
            }
        }
    } else {
        for (int j = 0; j < TILEMAP_HEIGHT; ++j) {
            for (int i = 0; i < TILEMAP_WIDTH; ++i) {
                SetTile(i, j, 0);
            }
        }
    }

    sig8_EndUndoableAction();
}

static void DrawTopButtons(void)
{
    if (sig8_DrawButton(2, 1, (Button) {
            .sprite = 5,
            .shortcut = "T",
            .hint = "PLACE TILES [T]"
    }, tool == TOOL_DRAW)) {
        tool = TOOL_DRAW;
    }

    if (sig8_DrawButton(11, 1, (Button) {
            .sprite = 6,
            .shortcut = "F",
            .hint = "FILL [F]"
    }, tool == TOOL_FILL)) {
        tool = TOOL_FILL;
    }

    if (sig8_DrawButton(20, 1, (Button) {
            .sprite = 7,
            .shortcut = "S",
            .hint = "SELECT [S]"
    }, tool == TOOL_SELECT)) {
        tool = TOOL_SELECT;
    }

    if (sig8_DrawButton(35, 1, (Button) {
            .sprite = 12,
            .shortcut = "Delete",
            .hint = "CLEAR [DEL]"
    }, false)) {
        Clear();
    }

    if (sig8_DrawButton(44, 1, (Button) {
            .sprite = 17,
            .shortcut = "Ctrl+C",
            .hint = "COPY [CTRL+C]"
    }, false)) {

    }

    if (sig8_DrawButton(53, 1, (Button) {
            .sprite = 18,
            .shortcut = "Ctrl+V",
            .hint = "PASTE [CTRL-V]"
    }, false)) {

    }

    if (sig8_DrawButton(SCREEN_WIDTH - 42, 1, (Button) {
            .sprite = 16,
            .shortcut = "G",
            .hint = "SHOW GRID [G]"
    }, showGrid)) {
        showGrid = !showGrid;
    }

    if (sig8_DrawButton(SCREEN_WIDTH - 34, 1, (Button) {
            .sprite = 1,
            .shortcut = "Tab",
            .hint = "SELECT TILE [TAB]"
    }, spriteTabOpen)) {
        spriteTabOpen = !spriteTabOpen;
    }

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
    FillRect(0, SCREEN_HEIGHT - TOOLBAR_SIZE + 1, SCREEN_WIDTH, TOOLBAR_SIZE - 1, DARK_BLUE);
    DrawTopButtons();

    UseFont(FONT_SMALL);

    DrawString(2, SCREEN_HEIGHT - 2, GRAY, sig8_StatusLine);

    if (spriteTabOpen) {
        DrawString(SCREEN_WIDTH - 23, SCREEN_HEIGHT - 2, RED, Format("#%03d", selectedSprite));
    } else if (selectedX != -1) {
        DrawString(SCREEN_WIDTH - 32, SCREEN_HEIGHT - 2, RED, Format("%03d:%03d", selectedX, selectedY));
    }
}

static void DrawTileSelector(void)
{
    Rect rect = {
            .x = SCREEN_WIDTH - SPRITESHEET_WIDTH * SPRITE_WIDTH - 2,
            .y = TOOLBAR_SIZE + 1,
            .width = SPRITESHEET_WIDTH * SPRITE_WIDTH + 2,
            .height = SPRITESHEET_HEIGHT * SPRITE_HEIGHT + 2
    };

    sig8_DrawSpriteSheet(
            rect.x + 1, rect.y + 1,
            spriteSheet, 1, &selectedSprite
    );

    sig8_StrokeRectR(sig8_AddBorder(rect, 1), WHITE);
}

static void DrawGrid(Rect rect, int freq, int color)
{
    for (int j = rect.y + Modulo(-cameraY, freq * SPRITE_HEIGHT); j < rect.y + rect.height; j += freq * SPRITE_HEIGHT) {
        DrawLine(rect.x, j, rect.x + rect.width, j, color);
    }

    for (int i = rect.x + Modulo(-cameraX, freq * SPRITE_WIDTH); i < rect.x + rect.width; i += freq * SPRITE_WIDTH) {
        DrawLine(i, rect.y, i, rect.y + rect.height, color);
    }
}

static void DrawSelection(void)
{
    int x = min(selection.x1, selection.x2) * SPRITE_WIDTH - cameraX;
    int y = min(selection.y1, selection.y2) * SPRITE_HEIGHT - cameraY + TOOLBAR_SIZE;
    int width = (diff(selection.x1, selection.x2) + 1) * SPRITE_WIDTH;
    int height = (diff(selection.y1, selection.y2) + 1) * SPRITE_HEIGHT;

    int anim = ticks / 15 % 4;

    for (int i = 0; i < width; ++i) {
        int c1 = (i % 4 != anim) * WHITE;
        int c2 = ((width - i + height) % 4 != anim) * WHITE;
        DrawPixel(i + x, y, c1);
        DrawPixel(i + x, y + height, c2);
    }

    for (int i = 0; i < height; ++i) {
        int c1 = ((height - i) % 4 != anim) * WHITE;
        int c2 = ((i + width) % 4 != anim) * WHITE;
        DrawPixel(x, i + y, c1);
        DrawPixel(x + width, i + y, c2);
    }
}

static void DrawTiles(void)
{
    Position position = GetMousePosition();
    Rect rect = {
            .x = 0,
            .y = TOOLBAR_SIZE,
            .width = spriteTabOpen ? (SCREEN_WIDTH - SPRITESHEET_WIDTH * SPRITE_WIDTH) : SCREEN_WIDTH,
            .height = SCREEN_HEIGHT - 2 * TOOLBAR_SIZE + 1
    };

    if (MouseJustReleased(MOUSE_RIGHT)) {
        // End drag
        isDragging = false;
    }

    if (sig8_IsMouseOver(rect)) {
        if (MouseJustPressed(MOUSE_RIGHT)) {
            // Begin drag
            isDragging = true;
            dragOriginX = cameraX + position.x;
            dragOriginY = cameraY + position.y;
        }

        if (isDragging) {
            cameraX = dragOriginX - position.x;
            cameraY = dragOriginY - position.y;
        }
    }

    UseSpriteSheet(spriteSheet);
    DrawTileMap(rect.x, rect.y, rect.width, rect.height, cameraX, cameraY);

    if (showGrid) {
        DrawGrid(rect, 1, DARK_RED);
    }

    DrawGrid(rect, 16, GRAY);

    if (sig8_IsMouseOver(rect)) {
        selectedX = Divide(position.x + cameraX - rect.x, SPRITE_WIDTH);
        selectedY = Divide(position.y + cameraY - rect.y, SPRITE_HEIGHT);

        Rect r = {
                .x = rect.x + selectedX * SPRITE_WIDTH - cameraX,
                .y = rect.y + selectedY * SPRITE_HEIGHT - cameraY,
                .width = SPRITE_WIDTH + 1,
                .height = SPRITE_HEIGHT + 1
        };

        if (tool == TOOL_DRAW) {
            DrawSpriteMask(r.x, r.y, selectedSprite, -1);

            sig8_StrokeRectR(r, WHITE);

            if (MousePressed(MOUSE_LEFT)) {
                sig8_BeginUndoableAction();
                if (KeyPressed("Shift")) {
                    SetTile(selectedX, selectedY, 0);
                } else {
                    SetTile(selectedX, selectedY, selectedSprite);
                }
                sig8_EndUndoableAction();
            }
        } else if (tool == TOOL_FILL) {
            DrawSpriteMask(r.x, r.y, selectedSprite, -1);

            if (MousePressed(MOUSE_LEFT)) {
                sig8_BeginUndoableAction();
                Fill(selectedX, selectedY, selectedSprite);
                sig8_EndUndoableAction();
            }
        } else if (tool == TOOL_SELECT) {
            sig8_Selection(&selection, selectedX, selectedY);
        }

        selectedX = Modulo(selectedX, TILEMAP_WIDTH);
        selectedY = Modulo(selectedY, TILEMAP_HEIGHT);
    } else {
        selectedX = -1;
        selectedY = -1;
    }

    UseSpriteSheet(sig8_EDITORS_SPRITESHEET);

    if (selection.active) {
        DrawSelection();
    }
}

void sig8_TileEditorInit(ManagedResource *what)
{
    sig8_Editing = what;
    UseTileMap(sig8_Editing->resource);
    spriteSheet = GetCurrentSpriteSheet();
    selectedX = selectedY = -1;
    spriteTabOpen = false;
    selection.active = false;
    sig8_HistoryClear();
}

static void HandleInput(void)
{
    if (KeyJustPressed("Escape")) {
        if (spriteTabOpen) {
            spriteTabOpen = false;
        } else if (selection.active) {
            selection.active = false;
        } else {
            SetCursorShape(CURSOR_ARROW);
            sig8_HistoryClear();
            sig8_LeaveEditor();
            return;
        }
    }
}

void sig8_TileEditorTick(void)
{
    ++ticks;
    UseSpriteSheet(sig8_EDITORS_SPRITESHEET);
    SetCursorShape(CURSOR_ARROW);
    sig8_StatusLine = "";

    ClearScreen(BLACK);
    DrawTiles();
    if (spriteTabOpen) {
        DrawTileSelector();
    }
    DrawStatusBar();
    HandleInput();
}
