#include "sig8tk.h"

void InvalidFileFormat()
{
    fprintf(stderr, "Error reading file.\n");
    Finalize();
    exit(EXIT_FAILURE);
}

void WriteResource(const Resource *resource, FILE *file)
{
    fprintf(file, "//!sig8 %d %s\n", resource->type, resource->name);

    if (resource->type == RESOURCE_SPRITE) {
        ResourceSprite sprite = resource->sprite;

        fprintf(file, "extern const Sprite %s;\n", resource->name);
        fprintf(file, "#ifdef SIG8_COMPILE_RESOURCES\n");
        fprintf(file, "static const uint8_t %s_Data[];\n", resource->name);
        fprintf(file, "static const SpriteDefinition %s_Def = {\n", resource->name);
        fprintf(file, "%d, %d, %s_Def\n", sprite.width, sprite.height, resource->name);
        fprintf(file, "};\n");
        fprintf(file, "const Sprite %s = &%s_Def;\n", resource->name, resource->name);

        fprintf(file, "static const uint8_t %s_Data[] = {\n", resource->name);
        for (int j = 0; j < sprite.height; ++j) {
            for (int i = 0; i < sprite.width; ++i) {
                fprintf(file, "%d, ", sprite.data[i + j * sprite.width]);
            }
            fprintf(file, "\n");
        }
        fprintf(file, "};\n");
        fprintf(file, "#endif\n");
    }
}

void ReadResourceSprite(ResourceSprite *sprite, FILE *file)
{
    char buf[MAX_LINE_LENGTH];
    fgets(buf, sizeof buf, file); // extern const Sprite NAME;
    fgets(buf, sizeof buf, file); // #ifdef SIG8_COMPILE_RESOURCES
    fgets(buf, sizeof buf, file); // static const uint8_t NAME_Data[];
    fgets(buf, sizeof buf, file); // static const SpriteDefinition NAME_Def = {
    fgets(buf, sizeof buf, file); // width, height, NAME_Def
    sscanf(buf, "%d, %d", &sprite->width, &sprite->height);
    sprite->frames = 1;
    if (sprite->width > MAX_SPRITE_DIMENSION || sprite->height > MAX_SPRITE_DIMENSION) {
        InvalidFileFormat();
    }
    sprite->data = malloc(sprite->width * sprite->height);
    fgets(buf, sizeof buf, file); // };
    fgets(buf, sizeof buf, file); // const Sprite NAME = &NAME_Def;
    fgets(buf, sizeof buf, file); // static const uint8_t NAME_Data[] = {
    int idx = 0;
    for (int j = 0; j < sprite->height; ++j) {
        fgets(buf, sizeof buf, file);
        char *s = strtok(buf, ", ");
        for (int i = 0; i < sprite->width; ++i) {
            sprite->data[idx++] = atoi(s);
            s = strtok(NULL, ", ");
        }
    }
    fgets(buf, sizeof buf, file); // };
    fgets(buf, sizeof buf, file); // #endif
}

Resource *ReadResources(FILE *file)
{
    char buf[MAX_LINE_LENGTH];
    int amount = 0;

    while (!feof(file)) {
        fgets(buf, sizeof buf, file);
        if (strncmp("//!sig8", buf, 7) == 0) {
            ++amount;
        }
    }

    Resource *result = malloc(sizeof(Resource) * (amount + 1));
    Resource *cur = result;
    rewind(file);

    while (!feof(file)) {
        fgets(buf, sizeof buf, file);
        if (strncmp("//!sig8", buf, 7) != 0) {
            continue;
        }

        sscanf(buf + 8, "%d %31s", &cur->type, cur->name);
        if (cur->type == RESOURCE_SPRITE) {
            ReadResourceSprite(&cur->sprite, file);
        }

        ++cur;
    }
    cur->type = RESOURCE_NONE;
    return result;
}
