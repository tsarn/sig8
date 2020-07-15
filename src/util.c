#include "sig8_internal.h"

char *scratchMemory = NULL;
size_t scratchMemorySize = 0;
size_t scratchMemoryCapacity = 0;

Color ColorFromHex(const char *hex)
{
    Color res = {.a = 255};
    assert(hex[0] == '#');
    int len = strlen(hex);

    unsigned value = strtol(hex + 1, NULL, 16);

    if (len == 4) {
        res.r = (value >> 8);
        res.r = (res.r << 4) | res.r;
        res.g = (value >> 4) & 0xf;
        res.g = (res.g << 4) | res.g;
        res.b = value & 0xf;
        res.b = (res.b << 4) | res.b;
    } else if (len == 7 || len == 9) {
        if (len == 9) {
            res.a = value & 0xff;
            value >>= 8;
        }
        res.r = (value >> 16);
        res.g = (value >> 8) & 0xff;
        res.b = value & 0xff;
    }

    return res;
}

FloatColor ColorToFloatColor(Color color)
{
    return (FloatColor){
        .r = color.r / 255.0f,
        .g = color.g / 255.0f,
        .b = color.b / 255.0f,
        .a = color.a / 255.0f,
    };
}

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
            fprintf(stderr, "TempAlloc: reallocation failed\n");
            Finalize();
            exit(EXIT_FAILURE);
        }
        scratchMemory = (char*)newPtr;
        scratchMemoryCapacity = allocSize;
    }

    return (void*)(scratchMemory + oldSize);
}

char *Format(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    int len = vsnprintf(NULL, 0, fmt, args);
    va_end(args);

    char *str = TempAlloc(len + 1);
    va_start(args, fmt);
    vsnprintf(str, len + 1, fmt, args);
    va_end(args);

    return str;
}

bool IsLightColor(int color)
{
    return !(color == BLACK || color == DARK_BLUE || color == BROWN ||
             color == DARK_PURPLE || color == DARK_GREEN || color == DARK_GRAY);
}
