#include "sig8tk.h"

static bool inputting;
static char input[MAX_RESOURCE_NAME];
static Resource *selectedResource;

void InitResourceSelector(const char *path)
{
    FILE *f = fopen(path, "r");
    ReadResources(f);
    fclose(f);

    inputting = false;
    selectedResource = NULL;
}

static void DrawSidebar(void)
{
    if (selectedResource) {
        BeginItem(23);
        BeginVBox(1);

        BeginItem(7);
        ColorLayout(WHITE);
        DrawString(1, 1, BLACK, "EDIT");
        if (Button(MOUSE_LEFT)) {
            EditResource(selectedResource);
        }
        EndLayout();

        BeginItem(7);
        ColorLayout(WHITE);
        DrawString(1, 1, BLACK, "CLONE");
        if (Button(MOUSE_LEFT)) {
        }
        EndLayout();

        BeginItem(7);
        ColorLayout(RED);
        DrawString(1, 1, WHITE, "DELETE");
        if (Button(MOUSE_LEFT)) {
            RemoveResource(selectedResource);
            selectedResource = NULL;
        }
        EndLayout();

        EndLayout();
        EndLayout();
    }

    BeginItem(7);
    ColorLayout(WHITE);
    DrawString(1, 1, BLACK, "+SPRITE");
    if (Button(MOUSE_LEFT)) {
        inputting = true;
        input[0] = 0;
    }
    EndLayout();
}

static void DrawResourceList(void)
{
    SetFont(FONT_3X5);

    BeginVBox(1);

    for (int i = 0; i < resourceCount; ++i) {
        Resource *res = &resources[i];
        BeginItem(7);
        ColorLayout(res == selectedResource ? DARK_GREEN : DARK_PURPLE);
        BeginMargin(1, 1, 1, 1);
        Text(res->name, WHITE, HALIGN_LEFT, VALIGN_MIDDLE);
        EndLayout();

        if (Button(MOUSE_LEFT)) {
            selectedResource = res;
        }

        EndLayout();
    }

    EndLayout();
}

void DrawResourceSelector(void)
{
    ClearScreen(BACKGROUND_COLOR);
    BeginUI();

    BeginVBox(0); // MainVBox
    DrawToolbar();

    BeginItem(-1);
    BeginHBox(0);

    BeginItem(SIDEBAR_WIDTH);
    ColorLayout(SIDEBAR_COLOR);
    BeginMargin(2, 2, 2, 2);
    BeginVBox(5);
    DrawSidebar();
    EndLayout();
    EndLayout();
    EndLayout();

    BeginItem(-1);
    ColorLayout(BACKGROUND_COLOR);
    DrawResourceList();
    EndLayout();

    EndLayout();
    EndLayout();

    EndLayout(); // MainVBox

    if (KeyJustPressed("Escape")) {
        if (inputting) {
            inputting = false;
        } else {
            Quit();
        }
    }

    if (inputting && KeyJustPressed("Return")) {
        CreateResource(input, RESOURCE_SPRITE);
        inputting = false;
        selectedResource = NULL;
    }

    if (inputting) {
        char key = GetJustPressedKey();
        int len = strlen(input);

        if (key == '-' || key == ' ') {
            key = '_';
        }

        if (key == '\b' && len > 0) {
            input[--len] = 0;
        }

        if ((key >= 'A' && key <= 'Z') || (key >= '0' && key <= '9') || key == '_') {
            if (len + 1 < MAX_RESOURCE_NAME) {
                input[len++] = key;
                input[len] = 0;
            }
        }

        FillRect(0, SCREEN_HEIGHT - 7, SCREEN_WIDTH, 7, BLACK);
        if (len == 0) {
            DrawString(1, SCREEN_HEIGHT - 6, INDIGO, "ENTER NAME");
        } else {
            DrawString(1, SCREEN_HEIGHT - 6, WHITE, input);
        }
    }
}
