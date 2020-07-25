#include "sig8_internal.h"

static char *scratchMemory = NULL;
static size_t scratchMemorySize = 0;
static size_t scratchMemoryCapacity = 0;

void* TempAlloc(size_t n)
{
    // round up for alignment
    n = (n + sizeof(size_t) - 1) / sizeof(size_t) * sizeof(size_t);

    size_t oldSize = scratchMemorySize;
    scratchMemorySize += n;

    if (scratchMemorySize > scratchMemoryCapacity)
    {
        size_t allocSize = scratchMemorySize * 2;
        void *newPtr = realloc(scratchMemory, allocSize);
        if (!newPtr) {
            puts("TempAlloc: reallocation failed");
            Finalize();
            exit(EXIT_FAILURE);
        }
        scratchMemory = (char*)newPtr;
        scratchMemoryCapacity = allocSize;
    }

    return (void*)(scratchMemory + oldSize);
}

static void ResetScratchMemory(void)
{
    scratchMemorySize = 0;
}

void sig8_InitAlloc(void)
{
    sig8_RegisterFrameCallback(ResetScratchMemory);
}
