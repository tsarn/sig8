#include "sig8_internal.h"

uint8_t *sig8_AllocateResource(ResourceType type, const char *path, int size)
{
#ifdef SIG8_COMPILE_EDITORS
    sig8_ManagedResource *res = calloc(1, size + sizeof(sig8_ManagedResource));
    res->type = type;
    if (!path) {
        res->path = NULL;
    } else {
        res->path = malloc(strlen(path) + 1);
        strcpy(res->path, path);
    }
    res->resource = ((uint8_t*)res) + sizeof(sig8_ManagedResource);
    return res->resource;
#else
    (void)type;
    (void)path;
    return calloc(1, size);
#endif
}

void sig8_FreeResource(uint8_t *resource)
{
#ifdef SIG8_COMPILE_EDITORS
    sig8_ManagedResource *res = sig8_GetManagedResource(resource);
    if (res->path) {
        free(res->path);
    }
    free(res);
#else
    free(resource);
#endif
}

#ifdef SIG8_COMPILE_EDITORS
sig8_ManagedResource *sig8_GetManagedResource(uint8_t *resource)
{
    return (sig8_ManagedResource*)(resource - sizeof(sig8_ManagedResource));
}

#endif
