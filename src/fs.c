#include "sig8_internal.h"

static const uint8_t *resourceBundle;
static const char *resourcePath;

const uint8_t *GetResourceBundle(void)
{
    return resourceBundle;
}

const char *GetResourcePath(void)
{
    return resourcePath;
}

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

char *ResolvePath(const char *path)
{
    char *fullPath = NULL;
    if (!strncmp(path, RESOURCE_PATH_PREFIX, strlen(RESOURCE_PATH_PREFIX))) {
        const char *stripped = path + strlen(RESOURCE_PATH_PREFIX);
        if (resourcePath) {
            fullPath = malloc(strlen(resourcePath) + strlen(stripped) + 1);
            sprintf(fullPath, "%s%s", resourcePath, stripped);
        }
    } else {
        fullPath = malloc(strlen(path) + 1);
        strcpy(fullPath, path);
    }

    return fullPath;
}

uint8_t *ReadFileContents(const char *path, int *size)
{
    if (!strncmp(path, RESOURCE_PATH_PREFIX, strlen(RESOURCE_PATH_PREFIX))) {
        const char *stripped = path + strlen(RESOURCE_PATH_PREFIX);

        const uint8_t *data = BundleSeek(stripped, size);
        if (!data) {
            if (resourcePath) {
                char *fullPath = malloc(strlen(resourcePath) + strlen(stripped) + 1);
                sprintf(fullPath, "%s%s", resourcePath, stripped);
                uint8_t *result = ReadFileContents(fullPath, size);
                free(fullPath);
                return result;
            } else {
                return NULL;
            }
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

void WriteFileContents(const char *path, const void *data, int size)
{
    if (!strncmp(path, RESOURCE_PATH_PREFIX, strlen(RESOURCE_PATH_PREFIX))) {
        if (!resourcePath) {
            return;
        }
    } else {
        FILE *file = fopen(path, "rb");
        if (!file) {
            return;
        }
        fwrite(data, 1, size, file);
        fclose(file);
    }
}
