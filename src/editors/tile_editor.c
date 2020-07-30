#include "editors.h"

#define TOOLBAR_SIZE 9

static SpriteSheet spriteSheet;

static int cameraX = -10;
static int cameraY = -10;
static int selectedX;
static int selectedY;

static bool isDragging;
static int dragOriginX;
static int dragOriginY;

static bool isSelectingTile;
static int selectedTile;

static bool showGrid;

static void Save(void)
{
}

static void DrawTopButtons(void)
{
    if (sig8_DrawButton(SCREEN_WIDTH - 42, 0, (Button) {
            .sprite = 16,
            .shortcut = "G",
            .hint = "SHOW GRID [G]"
    }, showGrid)) {
        showGrid = !showGrid;
    }

    if (sig8_DrawButton(SCREEN_WIDTH - 34, 0, (Button) {
            .sprite = 1,
            .shortcut = "Tab",
            .hint = "SELECT TILE [TAB]"
    }, isSelectingTile)) {
        isSelectingTile = !isSelectingTile;
    }

    if (sig8_DrawButton(SCREEN_WIDTH - 25, 0, (Button) {
            .sprite = 13,
            .shortcut = "Ctrl+Z",
            .hint = "UNDO [CTRL-Z]"
    }, false)) {
        sig8_Undo();
    }

    if (sig8_DrawButton(SCREEN_WIDTH - 17, 0, (Button) {
            .sprite = 14,
            .shortcut = "Ctrl+Y",
            .hint = "REDO [CTRL-Y]"
    }, false)) {
        sig8_Redo();
    }

    if (sig8_DrawButton(SCREEN_WIDTH - 9, 0, (Button) {
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
    FillRect(0, SCREEN_HEIGHT - TOOLBAR_SIZE, SCREEN_WIDTH, TOOLBAR_SIZE, DARK_BLUE);
    DrawTopButtons();

    UseFont(FONT_SMALL);

    DrawString(2, SCREEN_HEIGHT - 2, GRAY, sig8_StatusLine);

    if (isSelectingTile) {
        DrawString(SCREEN_WIDTH - 23, SCREEN_HEIGHT - 2, RED, Format("#%03d", selectedTile));
    } else if (selectedX != -1) {
        DrawString(SCREEN_WIDTH - 32, SCREEN_HEIGHT - 2, RED, Format("%03d:%03d", selectedX, selectedY));
    }

    UseFont(FONT_MEDIUM);
    if (sig8_Editing->path) {
        DrawString(2, 8, GRAY, sig8_Editing->path);
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
            spriteSheet, 1, &selectedTile
    );

    sig8_StrokeRectR(sig8_AddBorder(rect, 1), WHITE);
}

static void DrawGrid(Rect rect)
{
    for (int j = rect.y + Modulo(-cameraY, SPRITE_HEIGHT); j < rect.y + rect.height; j += SPRITE_HEIGHT) {
        DrawLine(rect.x, j, rect.x + rect.width, j, DARK_RED);
    }

    for (int i = rect.x + Modulo(-cameraX, SPRITE_WIDTH); i < rect.x + rect.width; i += SPRITE_WIDTH) {
        DrawLine(i, rect.y, i, rect.y + rect.height, DARK_RED);
    }
}

static void DrawTiles(void)
{
    Position position = GetMousePosition();
    Rect rect = {
            .x = 0,
            .y = TOOLBAR_SIZE,
            .width = isSelectingTile ? (SCREEN_WIDTH - SPRITESHEET_WIDTH * SPRITE_WIDTH) : SCREEN_WIDTH,
            .height = SCREEN_HEIGHT - 2 * TOOLBAR_SIZE
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
        DrawGrid(rect);
    }

    if (sig8_IsMouseOver(rect)) {
        selectedX = Divide(position.x + cameraX - rect.x, SPRITE_WIDTH);
        selectedY = Divide(position.y + cameraY - rect.y, SPRITE_HEIGHT);

        Rect r = {
                .x = rect.x + selectedX * SPRITE_WIDTH - cameraX,
                .y = rect.y + selectedY * SPRITE_HEIGHT - cameraY,
                .width = SPRITE_WIDTH + 1,
                .height = SPRITE_HEIGHT + 1
        };

        DrawSprite(r.x, r.y, GetTile(selectedX, selectedY));

        sig8_StrokeRectR(r, WHITE);

        selectedX = Modulo(selectedX, TILEMAP_WIDTH);
        selectedY = Modulo(selectedY, TILEMAP_HEIGHT);

        if (MousePressed(MOUSE_LEFT)) {
            SetTile(selectedX, selectedY, selectedTile);
        }
    } else {
        selectedX = -1;
        selectedY = -1;
    }

    UseSpriteSheet(sig8_EDITORS_SPRITESHEET);
}

void sig8_TileEditorInit(ManagedResource *what)
{
    sig8_Editing = what;
    UseTileMap(sig8_Editing->resource);
    spriteSheet = GetCurrentSpriteSheet();
    selectedX = selectedY = -1;
    sig8_HistoryClear();
}

void sig8_TileEditorTick(void)
{
    UseSpriteSheet(sig8_EDITORS_SPRITESHEET);
    SetCursorShape(CURSOR_ARROW);
    sig8_StatusLine = "";

    ClearScreen(BLACK);
    DrawTiles();
    if (isSelectingTile) {
        DrawTileSelector();
    }
    DrawStatusBar();
}
