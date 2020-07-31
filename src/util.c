#include "sig8_internal.h"

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
