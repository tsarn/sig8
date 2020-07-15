#include "sig8tk.h"

static ResourceSprite sprite;
static int fg, bg;
static int zoom;
static int brush;

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
    bg = BLACK;
    fg = WHITE;
    //sprite = NewSprite(8, 8);
    FILE *f = fopen("sig8tk_resources.h", "r");
    Resource *resources = ReadResources(f);
    sprite = resources->sprite;
    fclose(f);
    zoom = 6;
    brush = 1;
}

int GetPixel(int x, int y)
{
    if (x < 0 || y < 0 || x >= sprite.width || y >= sprite.height) {
        return TRANSPARENT;
    }
    return sprite.data[y * sprite.width + x];
}

void SetPixel(int x, int y, int color)
{
    if (x < 0 || y < 0 || x >= sprite.width || y >= sprite.height) {
        return;
    }
    sprite.data[y * sprite.width + x] = color;
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

            if (color == fg) {
                DrawPixel(0, 0, IsLightColor(color) ? BLACK : WHITE);
                DrawPixel(1, 0, IsLightColor(color) ? BLACK : WHITE);
                DrawPixel(2, 0, IsLightColor(color) ? BLACK : WHITE);
                DrawPixel(0, 1, IsLightColor(color) ? BLACK : WHITE);
                DrawPixel(1, 1, IsLightColor(color) ? BLACK : WHITE);
                DrawPixel(0, 2, IsLightColor(color) ? BLACK : WHITE);
            }

            if (color == bg) {
                DrawPixel(6, 6, IsLightColor(color) ? BLACK : WHITE);
                DrawPixel(5, 6, IsLightColor(color) ? BLACK : WHITE);
                DrawPixel(6, 5, IsLightColor(color) ? BLACK : WHITE);
            }

            if (Button(MOUSE_LEFT)) {
                fg = color;
            }

            if (Button(MOUSE_RIGHT)) {
                bg = color;
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

void DrawEditedSprite()
{
    BeginCenter(zoom * sprite.width + 2,
            zoom * sprite.height + 2);

    ColorLayout(WHITE);
    BeginMargin(1, 1, 1, 1);

    BeginVBox(0);

    for (int j = 0; j < sprite.height; ++j) {
        BeginItem(zoom);
        BeginHBox(0);

        for (int i = 0; i < sprite.width; ++i) {
            BeginItem(zoom);
            ColorLayout(GetPixel(i, j));

            EndLayout();
        }

        EndLayout();
        EndLayout();
    }

    EndLayout();

    if (Button(MOUSE_HOVER)) {
        int cx = GetMousePosition().x - GetArea().x;
        int cy = GetMousePosition().y - GetArea().y;
        int t = zoom * brush;

        cx -= brush * zoom / 2;
        cy -= brush * zoom / 2;

        cx = (cx + zoom / 2) / zoom;
        cy = (cy + zoom / 2) / zoom;

        StrokeRect(cx * zoom - 1, cy * zoom - 1, t + 2, t + 2, WHITE);
        StrokeRect(cx * zoom, cy * zoom, t, t, BLACK);

        if (MousePressed(MOUSE_LEFT)) {
            for (int i = cx; i < cx + brush; ++i) {
                for (int j = cy; j < cy + brush; ++j) {
                    SetPixel(i, j, fg);
                }
            }
        }

        if (MousePressed(MOUSE_RIGHT)) {
            for (int i = cx; i < cx + brush; ++i) {
                for (int j = cy; j < cy + brush; ++j) {
                    SetPixel(i, j, bg);
                }
            }
        }
    }

    EndLayout();
    EndLayout();
}

void DrawSpriteEditor(void)
{
    if (KeyJustPressed("1")) brush = 1;
    if (KeyJustPressed("2")) brush = 2;
    if (KeyJustPressed("3")) brush = 3;
    if (KeyJustPressed("4")) brush = 4;

    if (KeyJustPressed("S")) {
        FILE *f = fopen("sig8tk_resources.h", "w");
        Resource res;
        strcpy(res.name, "TEST");
        res.type = RESOURCE_SPRITE;
        res.sprite = sprite;
        WriteResource(&res, f);
        fclose(f);
    }

    if (KeyJustPressed("Escape")) {
        Quit();
    }

    ClearScreen(BACKGROUND_COLOR);
    BeginUI();

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
    DrawEditedSprite();
    EndLayout();

    EndLayout();
    EndLayout();

    EndLayout(); // MainVBox
}
