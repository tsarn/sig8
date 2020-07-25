#include "sig8_internal.h"

static const uint8_t *resourceBundle;
static const char *resourcePath;

void UseResourceBundle(const uint8_t *bundle)
{
    resourceBundle = bundle;
}

void UseResourcePath(const char *path)
{
    resourcePath = path;
}

static const uint8_t *BundleSeek(const char *name, int *size)
{
    if (!resourceBundle) {
        return NULL;
    }

    const uint8_t *ptr = resourceBundle;
    while (*ptr) {
        bool ok = false;
        if (!strcmp((const char*)ptr, name)) {
            ok = true;
        }

        ptr = ptr + strlen((const char*)ptr) + 1;
        *size = 0;
        for (int i = 0; i < 4; ++i) {
            *size |= (*ptr++ << (8 * i));
        }

        if (ok) {
            return ptr;
        }

        ptr += *size;
    }
    return NULL;
}

uint8_t *ReadFileContents(const char *path, int *size)
{
    if (!strncmp(path, RESOURCE_PATH_PREFIX, strlen(RESOURCE_PATH_PREFIX))) {
        const char *stripped = path + strlen(RESOURCE_PATH_PREFIX);
        if (resourcePath) {
            char *fullPath = malloc(strlen(resourcePath) + strlen(stripped) + 4);
            sprintf(fullPath, "%s/%s", resourcePath, stripped);
            uint8_t *result = ReadFileContents(fullPath, size);
            free(fullPath);
            return result;
        }

        const uint8_t *data = BundleSeek(stripped, size);
        if (!data) {
            return NULL;
        }

        uint8_t *result = malloc(*size);
        memcpy(result, data, *size);
        return result;
    } else {
        FILE *file = fopen(path, "rb");
        if (!file) {
            return NULL;
        }

        fseek(file, 0L, SEEK_END);
        *size = ftell(file);
        fseek(file, 0L, SEEK_SET);

        uint8_t *result = malloc(*size);
        fread(result, *size, 1, file);
        fclose(file);
        return result;
    }
}
