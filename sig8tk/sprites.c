#include "sig8tk.h"

#define SPR_X 16
#define SPR_Y 16

static SpriteSheet editing;
static int selected;

static void DrawSpriteSheet(void)
{
    Rect rect = {
            .y = TOOLBAR_SIZE + 3,
            .width = SPR_X * SPRITE_WIDTH,
            .height = SPR_Y * SPRITE_HEIGHT,
    };
    rect.x = SCREEN_WIDTH - 1 - rect.width;

    FillRectRB(rect, BLACK, GRAY);

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
                    selected = idx;
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
                .width = SPRITE_WIDTH,
                .height = SPRITE_HEIGHT
        };
        StrokeRectR(AddBorder(r, 2), WHITE);
        StrokeRectR(AddBorder(r, 1), BLACK);
    }
}

static void DrawStatusString(void)
{
    char *string = Format("#%03d", selected);
    DrawString(SCREEN_WIDTH - 23, 8, RED, string);
}

void SpritesInit(void)
{
    editing = MAIN_SPRITESHEET;
    selected = 0;
}

void SpritesTick(void)
{
    ClearScreen(INDIGO);
    DrawToolbar();
    DrawSpriteSheet();

    DrawStatusString();
}
