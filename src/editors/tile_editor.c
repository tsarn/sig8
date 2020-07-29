#include "editors.h"

#define TOOLBAR_SIZE 10

static SpriteSheet spriteSheet;

static int cameraX = -10;
static int cameraY = -10;
static int selectedX;
static int selectedY;

static bool isDragging;
static int dragOriginX;
static int dragOriginY;

static void DrawStatusBar(void)
{
    FillRect(0, 0, SCREEN_WIDTH, TOOLBAR_SIZE, DARK_BLUE);
    FillRect(0, SCREEN_HEIGHT - TOOLBAR_SIZE, SCREEN_WIDTH, TOOLBAR_SIZE, DARK_BLUE);

    UseFont(FONT_SMALL);

    if (selectedX != -1) {
        DrawString(SCREEN_WIDTH - 32, SCREEN_HEIGHT - 2, RED, Format("%03d:%03d", selectedX, selectedY));
    }

    UseFont(FONT_MEDIUM);
    if (sig8_Editing->path) {
        DrawString(2, 8, GRAY, sig8_Editing->path);
    }
}

static void DrawTiles(void)
{

    Position position = GetMousePosition();
    Rect rect = {
            .x = 0,
            .y = TOOLBAR_SIZE,
            .width = SCREEN_WIDTH,
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
    UseSpriteSheet(sig8_EDITORS_SPRITESHEET);

    if (sig8_IsMouseOver(rect)) {
        selectedX = Divide(position.x + cameraX - rect.x, SPRITE_WIDTH);
        selectedY = Divide(position.y + cameraY - rect.y, SPRITE_HEIGHT);

        Rect r = {
                .x = rect.x + selectedX * SPRITE_WIDTH - cameraX,
                .y = rect.y + selectedY * SPRITE_HEIGHT - cameraY,
                .width = SPRITE_WIDTH - 1,
                .height = SPRITE_HEIGHT - 1
        };

        sig8_StrokeRectR(sig8_AddBorder(r, 2), WHITE);
        sig8_StrokeRectR(sig8_AddBorder(r, 1), BLACK);

        selectedX = Modulo(selectedX, TILEMAP_WIDTH);
        selectedY = Modulo(selectedY, TILEMAP_HEIGHT);
    } else {
        selectedX = -1;
        selectedY = -1;
    }
}

void sig8_TileEditorInit(ManagedResource *what)
{
    sig8_Editing = what;
    UseTileMap(sig8_Editing->resource);
    spriteSheet = GetCurrentSpriteSheet();
    selectedX = selectedY = -1;
}

void sig8_TileEditorTick(void)
{
    UseSpriteSheet(sig8_EDITORS_SPRITESHEET);
    SetCursorShape(CURSOR_ARROW);
    ClearScreen(BLACK);
    DrawTiles();
    DrawStatusBar();
}
