#include "sig8_internal.h"

static const uint8_t fontData5x7[] = {
// char code 32
        0x0, 0x0, 0x0, 0x0, 0x0,
// char code 33
        0x0, 0x0, 0x5f, 0x0, 0x0,
// char code 34
        0x0, 0x7, 0x0, 0x7, 0x0,
// char code 35
        0x14, 0x7f, 0x14, 0x7f, 0x14,
// char code 36
        0x24, 0x2a, 0x7f, 0x2a, 0x12,
// char code 37
        0x23, 0x13, 0x8, 0x64, 0x62,
// char code 38
        0x36, 0x49, 0x56, 0x20, 0x50,
// char code 39
        0x0, 0x8, 0x7, 0x3, 0x0,
// char code 40
        0x0, 0x1c, 0x22, 0x41, 0x0,
// char code 41
        0x0, 0x41, 0x22, 0x1c, 0x0,
// char code 42
        0x2a, 0x1c, 0x7f, 0x1c, 0x2a,
// char code 43
        0x8, 0x8, 0x3e, 0x8, 0x8,
// char code 44
        0x0, 0x0, 0x70, 0x30, 0x0,
// char code 45
        0x8, 0x8, 0x8, 0x8, 0x8,
// char code 46
        0x0, 0x0, 0x60, 0x60, 0x0,
// char code 47
        0x20, 0x10, 0x8, 0x4, 0x2,
// char code 48
        0x3e, 0x51, 0x49, 0x45, 0x3e,
// char code 49
        0x0, 0x42, 0x7f, 0x40, 0x0,
// char code 50
        0x72, 0x49, 0x49, 0x49, 0x46,
// char code 51
        0x21, 0x41, 0x49, 0x4d, 0x33,
// char code 52
        0x18, 0x14, 0x12, 0x7f, 0x10,
// char code 53
        0x27, 0x45, 0x45, 0x45, 0x39,
// char code 54
        0x3c, 0x4a, 0x49, 0x49, 0x31,
// char code 55
        0x41, 0x21, 0x11, 0x9, 0x7,
// char code 56
        0x36, 0x49, 0x49, 0x49, 0x36,
// char code 57
        0x46, 0x49, 0x49, 0x29, 0x1e,
// char code 58
        0x0, 0x0, 0x14, 0x0, 0x0,
// char code 59
        0x0, 0x40, 0x34, 0x0, 0x0,
// char code 60
        0x0, 0x8, 0x14, 0x22, 0x41,
// char code 61
        0x14, 0x14, 0x14, 0x14, 0x14,
// char code 62
        0x0, 0x41, 0x22, 0x14, 0x8,
// char code 63
        0x2, 0x1, 0x59, 0x9, 0x6,
// char code 64
        0x3e, 0x41, 0x5d, 0x59, 0x4e,
// char code 65
        0x7c, 0x12, 0x11, 0x12, 0x7c,
// char code 66
        0x7f, 0x49, 0x49, 0x49, 0x36,
// char code 67
        0x3e, 0x41, 0x41, 0x41, 0x22,
// char code 68
        0x7f, 0x41, 0x41, 0x41, 0x3e,
// char code 69
        0x7f, 0x49, 0x49, 0x49, 0x41,
// char code 70
        0x7f, 0x9, 0x9, 0x9, 0x1,
// char code 71
        0x3e, 0x41, 0x41, 0x51, 0x73,
// char code 72
        0x7f, 0x8, 0x8, 0x8, 0x7f,
// char code 73
        0x0, 0x41, 0x7f, 0x41, 0x0,
// char code 74
        0x20, 0x40, 0x41, 0x3f, 0x1,
// char code 75
        0x7f, 0x8, 0x14, 0x22, 0x41,
// char code 76
        0x7f, 0x40, 0x40, 0x40, 0x40,
// char code 77
        0x7f, 0x2, 0x1c, 0x2, 0x7f,
// char code 78
        0x7f, 0x4, 0x8, 0x10, 0x7f,
// char code 79
        0x3e, 0x41, 0x41, 0x41, 0x3e,
// char code 80
        0x7f, 0x9, 0x9, 0x9, 0x6,
// char code 81
        0x3e, 0x41, 0x51, 0x21, 0x5e,
// char code 82
        0x7f, 0x9, 0x19, 0x29, 0x46,
// char code 83
        0x26, 0x49, 0x49, 0x49, 0x32,
// char code 84
        0x3, 0x1, 0x7f, 0x1, 0x3,
// char code 85
        0x3f, 0x40, 0x40, 0x40, 0x3f,
// char code 86
        0x1f, 0x20, 0x40, 0x20, 0x1f,
// char code 87
        0x3f, 0x40, 0x38, 0x40, 0x3f,
// char code 88
        0x63, 0x14, 0x8, 0x14, 0x63,
// char code 89
        0x3, 0x4, 0x78, 0x4, 0x3,
// char code 90
        0x61, 0x59, 0x49, 0x4d, 0x43,
// char code 91
        0x0, 0x7f, 0x41, 0x41, 0x41,
// char code 92
        0x2, 0x4, 0x8, 0x10, 0x20,
// char code 93
        0x0, 0x41, 0x41, 0x41, 0x7f,
// char code 94
        0x4, 0x2, 0x1, 0x2, 0x4,
// char code 95
        0x40, 0x40, 0x40, 0x40, 0x40,
// char code 96
        0x0, 0x3, 0x7, 0x8, 0x0,
// char code 97
        0x20, 0x54, 0x54, 0x78, 0x40,
// char code 98
        0x7f, 0x28, 0x44, 0x44, 0x38,
// char code 99
        0x38, 0x44, 0x44, 0x44, 0x28,
// char code 100
        0x38, 0x44, 0x44, 0x28, 0x7f,
// char code 101
        0x38, 0x54, 0x54, 0x54, 0x18,
// char code 102
        0x0, 0x8, 0x7e, 0x9, 0x2,
// char code 103
        0x6, 0x49, 0x49, 0x45, 0x3f,
// char code 104
        0x7f, 0x8, 0x4, 0x4, 0x78,
// char code 105
        0x0, 0x44, 0x7d, 0x40, 0x0,
// char code 106
        0x20, 0x40, 0x40, 0x3d, 0x0,
// char code 107
        0x7f, 0x10, 0x28, 0x44, 0x0,
// char code 108
        0x0, 0x41, 0x7f, 0x40, 0x0,
// char code 109
        0x7c, 0x4, 0x78, 0x4, 0x78,
// char code 110
        0x7c, 0x8, 0x4, 0x4, 0x78,
// char code 111
        0x38, 0x44, 0x44, 0x44, 0x38,
// char code 112
        0x7c, 0x18, 0x24, 0x24, 0x18,
// char code 113
        0x18, 0x24, 0x24, 0x18, 0x7c,
// char code 114
        0x7c, 0x8, 0x4, 0x4, 0x8,
// char code 115
        0x48, 0x54, 0x54, 0x54, 0x24,
// char code 116
        0x4, 0x4, 0x3f, 0x44, 0x24,
// char code 117
        0x3c, 0x40, 0x40, 0x20, 0x7c,
// char code 118
        0x1c, 0x20, 0x40, 0x20, 0x1c,
// char code 119
        0x3c, 0x40, 0x30, 0x40, 0x3c,
// char code 120
        0x44, 0x28, 0x10, 0x28, 0x44,
// char code 121
        0x4c, 0x50, 0x20, 0x10, 0xc,
// char code 122
        0x44, 0x64, 0x54, 0x4c, 0x44,
// char code 123
        0x0, 0x8, 0x36, 0x41, 0x0,
// char code 124
        0x0, 0x0, 0x77, 0x0, 0x0,
// char code 125
        0x0, 0x41, 0x36, 0x8, 0x0,
// char code 126
        0x2, 0x1, 0x2, 0x4, 0x2,
// char code 127
        0x3c, 0x26, 0x23, 0x26, 0x3c,
};

static const FontDefinition fontDefinition5x7 = {
        .firstCharCode = 32,
        .lastCharCode = 127,
        .width = 5,
        .height = 7,
        .horizontalStep = 1,
        .verticalStep = 1,
        .fontData = fontData5x7
};

const Font font5x7 = &fontDefinition5x7;
