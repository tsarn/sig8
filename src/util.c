#include "sig8_internal.h"


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
