#include "sig8tk.h"

SpriteEditor spriteEditor;

ResourceSprite NewSprite(int w, int h)
{
    uint8_t *data = malloc(w * h);
    memset(data, 0, w * h);
    return (ResourceSprite){
        .width = w,
        .height = h,
        .frames = 1,
        .data = data
    };
}

void InitSpriteEditor(void)
{
    spriteEditor.bg = BLACK;
    spriteEditor.fg = WHITE;
}

bool IsLightColor(int color)
{
    return !(color == BLACK || color == DARK_BLUE ||
    color == DARK_PURPLE || color == DARK_GREEN);
}

void DrawColorPicker(void)
{
    BeginItem(37);
    BeginMargin(3, 5, 0, 5);
    ColorLayout(WHITE);
    DrawHLine(0, GetAreaHeight() - 1, GetAreaWidth(), BLACK);

    BeginVBox(1);

    BeginItem(0);
    EndLayout();

    for (int j = 0; j < 4; ++j) {
        BeginItem(7);
        BeginHBox(1);
        BeginItem(0);
        EndLayout();

        for (int i = 0; i < 4; ++i) {
            int color = i + j * 4;
            BeginItem(7);
            ColorLayout(color);

            if (color == spriteEditor.fg) {
                DrawPixel(0, 0, IsLightColor(color) ? BLACK : WHITE);
                DrawPixel(1, 0, IsLightColor(color) ? BLACK : WHITE);
                DrawPixel(2, 0, IsLightColor(color) ? BLACK : WHITE);
                DrawPixel(0, 1, IsLightColor(color) ? BLACK : WHITE);
                DrawPixel(1, 1, IsLightColor(color) ? BLACK : WHITE);
                DrawPixel(0, 2, IsLightColor(color) ? BLACK : WHITE);
            }

            if (color == spriteEditor.bg) {
                DrawPixel(6, 6, IsLightColor(color) ? BLACK : WHITE);
                DrawPixel(5, 6, IsLightColor(color) ? BLACK : WHITE);
                DrawPixel(6, 5, IsLightColor(color) ? BLACK : WHITE);
            }

            if (Clickable(MOUSE_LEFT)) {
                spriteEditor.fg = color;
            }

            if (Clickable(MOUSE_RIGHT)) {
                spriteEditor.bg = color;
            }

            EndLayout();
        }

        EndLayout();
        EndLayout();
    }

    EndLayout();

    EndLayout();
    EndLayout();
}

void DrawSpriteEditor(void)
{
    ClearScreen(BACKGROUND_COLOR);
    ResetLayout();

    BeginVBox(0); // MainVBox

    BeginItem(7); // Toolbar
    ColorLayout(TOOLBAR_COLOR);
    EndLayout(); // Toolbar

    BeginItem(-1);
    BeginHBox(0);

    BeginItem(43);
    ColorLayout(DARK_GRAY);
    BeginVBox(5);
    DrawColorPicker();
    EndLayout();
    EndLayout();

    BeginItem(-1);
    ColorLayout(BACKGROUND_COLOR);
    EndLayout();

    EndLayout();
    EndLayout();

    EndLayout(); // MainVBox
}
