#include "sig8_internal.h"

static char *scratchMemory = NULL;
static int scratchMemorySize = 0;
static int scratchMemoryCapacity = 0;
static int scratchMemoryNeeds = 0;

static void** pointersToFree = NULL;
static int pointersToFreeSize = 0;
static int pointersToFreeCapacity = 0;

static void *TempMalloc(int n)
{
    void *ptr = malloc(n);

    ++pointersToFreeSize;
    if (pointersToFreeSize > pointersToFreeCapacity) {
        pointersToFreeCapacity = 2 * pointersToFreeSize;
        pointersToFree = realloc(pointersToFree, pointersToFreeCapacity * sizeof(void*));
    }
    pointersToFree[pointersToFreeSize - 1] = ptr;

    return ptr;
}

void* TempAlloc(int n)
{
    // round up for alignment
    n = (n + sizeof(size_t) - 1) / sizeof(size_t) * sizeof(size_t);

    size_t oldSize = scratchMemorySize;

    if (scratchMemorySize + n > scratchMemoryCapacity)
    {
        // out of memory
        scratchMemoryNeeds += n;
        return TempMalloc(n);
    }

    scratchMemorySize += n;

    return (void*)(scratchMemory + oldSize);
}

static void ResetScratchMemory(void)
{
    if (scratchMemoryNeeds > scratchMemoryCapacity) {
        scratchMemoryCapacity = 2 * scratchMemoryNeeds;
        if (scratchMemory) {
            free(scratchMemory);
        }
        scratchMemory = malloc(scratchMemoryCapacity);
    }

    for (int i = 0; i < pointersToFreeSize; ++i) {
        free(pointersToFree[i]);
    }

    pointersToFreeSize = 0;
    scratchMemorySize = 0;
}

void sig8_InitAlloc(void)
{
    sig8_RegisterCallback(FRAME_EVENT, ResetScratchMemory);
}
