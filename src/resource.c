#include "sig8_internal.h"

void *sig8_AllocateResource(ResourceType type, const char *path, int size)
{
#ifdef SIG8_COMPILE_EDITORS
    ManagedResource *res = calloc(1, size + sizeof(ManagedResource));
    res->type = type;
    res->size = size;
    if (!path) {
        res->path = NULL;
    } else {
        res->path = malloc(strlen(path) + 1);
        strcpy(res->path, path);
    }
    res->resource = ((uint8_t*)res) + sizeof(ManagedResource);
    return res->resource;
#else
    (void)type;
    (void)path;
    return calloc(1, size);
#endif
}

void sig8_FreeResource(void *resource)
{
#ifdef SIG8_COMPILE_EDITORS
    ManagedResource *res = sig8_GetManagedResource((uint8_t*)resource);
    if (res->path) {
        free(res->path);
    }
    free(res);
#else
    free(resource);
#endif
}

#ifdef SIG8_COMPILE_EDITORS
ManagedResource *sig8_GetManagedResource(uint8_t *resource)
{
    return (ManagedResource*)(resource - sizeof(ManagedResource));
}
#endif
