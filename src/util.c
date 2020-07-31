#include "sig8_internal.h"

Color ColorFromHex(const char *hex)
{
    Color res;
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
    } else if (len == 7) {
        res.r = (value >> 16);
        res.g = (value >> 8) & 0xff;
        res.b = value & 0xff;
    }

    return res;
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

int Modulo(int a, int b)
{
    return (a % b + b) % b;
}

int Divide(int a, int b)
{
    return (a - Modulo(a, b)) / b;
}
