#include "editors.h"

void sig8_SaveGzipped(void)
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
