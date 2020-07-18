#include "sig8tk.h"

static ResourceType currentlyEditing = RESOURCE_NONE;
const char *statusString = "";
Resource *editedResource = NULL;

void DrawToolbar(void)
{
    BeginItem(TOOLBAR_HEIGHT);
    ColorLayout(TOOLBAR_COLOR);

    BeginHBox(1);
    BeginItem(0);
    EndLayout();

    RemapColor(BLACK, currentlyEditing == RESOURCE_NONE ? SIDEBAR_COLOR : TRANSPARENT);
    RemapColor(WHITE, currentlyEditing == RESOURCE_NONE ? WHITE : ICON_COLOR);
    BeginItem(8);
    DrawSprite(0, 0, LIST);
    if (Button(MOUSE_LEFT)) {
        EditResource(NULL);
    }
    EndLayout();

    RemapColor(BLACK, currentlyEditing == RESOURCE_SPRITE ? SIDEBAR_COLOR : TRANSPARENT);
    RemapColor(WHITE, currentlyEditing == RESOURCE_SPRITE ? WHITE : ICON_COLOR);
    BeginItem(8);
    DrawSprite(0, 0, SPRITE);
    EndLayout();

    ResetColors();

    EndLayout();
    EndLayout();
}

void DrawStatusString(void)
{
    PushArea();
    ResetArea();
    SetArea(0, 0, SCREEN_WIDTH - 3, TOOLBAR_HEIGHT);
    Text(statusString, ICON_COLOR, HALIGN_RIGHT, VALIGN_MIDDLE);
    PopArea();
}

void EditResource(Resource *resource)
{
    if (!resource || resource->type == RESOURCE_NONE) {
        editedResource = NULL;
        currentlyEditing = RESOURCE_NONE;
        return;
    }

    editedResource = resource;
    currentlyEditing = resource->type;

    if (resource->type == RESOURCE_SPRITE) {
        InitSpriteEditor();
    }
}

int main(int argc, char **argv)
{
    Initialize("sig8tk");

    const char *path = "../../sig8tk/sig8tk_resources.h";

    InitResourceSelector(path);

    while (!ShouldQuit()) {
        if (KeyJustPressed("Ctrl+S")) {
            FILE *f = fopen(path, "w");
            WriteResources(f);
            fclose(f);
        }

        switch (currentlyEditing) {
        case RESOURCE_NONE:
            DrawResourceSelector();
            break;

        case RESOURCE_SPRITE:
            DrawSpriteEditor();
            break;
        }

        if (KeyJustPressed("Escape")) {
            if (currentlyEditing != RESOURCE_NONE) {
                EditResource(NULL);
            }
        }
    }

    Finalize();
    return 0;
}
