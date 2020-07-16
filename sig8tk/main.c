#include "sig8tk.h"

static ResourceType currentlyEditing = RESOURCE_NONE;
Resource *editedResource = NULL;

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

    const char *path = "./sig8tk_resources.h";

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
