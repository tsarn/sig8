#include "sig8_internal.h"

static const uint8_t data[] = {
// char code 32
        3, 6, 0, 0, 0, 0, 0,
// char code 33
        1, 6, 23, 0, 0, 0, 0,
// char code 34
        3, 6, 3, 0, 3, 0, 0,
// char code 35
        5, 6, 10, 31, 10, 31, 10,
// char code 36
        4, 6, 0, 22, 63, 26, 0,
// char code 37
        5, 6, 1, 24, 4, 3, 16,
// char code 38
        5, 6, 10, 21, 21, 8, 20,
// char code 39
        1, 6, 3, 0, 0, 0, 0,
// char code 40
        2, 6, 14, 17, 0, 0, 0,
// char code 41
        2, 6, 17, 14, 0, 0, 0,
// char code 42
        3, 6, 10, 4, 10, 0, 0,
// char code 43
        3, 6, 4, 14, 4, 0, 0,
// char code 44
        2, 6, 32, 48, 0, 0, 0,
// char code 45
        3, 6, 4, 4, 4, 0, 0,
// char code 46
        1, 6, 16, 0, 0, 0, 0,
// char code 47
        5, 6, 16, 8, 4, 2, 1,
// char code 48
        4, 6, 14, 17, 17, 14, 0,
// char code 49
        3, 6, 18, 31, 16, 0, 0,
// char code 50
        4, 6, 25, 21, 21, 18, 0,
// char code 51
        4, 6, 17, 21, 21, 10, 0,
// char code 52
        4, 6, 12, 10, 31, 8, 0,
// char code 53
        4, 6, 23, 21, 21, 9, 0,
// char code 54
        4, 6, 14, 21, 21, 8, 0,
// char code 55
        4, 6, 1, 25, 5, 3, 0,
// char code 56
        4, 6, 10, 21, 21, 10, 0,
// char code 57
        4, 6, 2, 21, 21, 14, 0,
// char code 58
        1, 6, 10, 0, 0, 0, 0,
// char code 59
        1, 6, 26, 0, 0, 0, 0,
// char code 60
        3, 6, 4, 10, 17, 0, 0,
// char code 61
        4, 6, 10, 10, 10, 10, 0,
// char code 62
        3, 6, 17, 10, 4, 0, 0,
// char code 63
        3, 6, 1, 21, 2, 0, 0,
// char code 64
        5, 6, 30, 17, 13, 21, 14,
// char code 65
        4, 6, 30, 9, 9, 31, 0,
// char code 66
        4, 6, 31, 21, 21, 10, 0,
// char code 67
        3, 6, 14, 17, 17, 0, 0,
// char code 68
        4, 6, 31, 17, 17, 14, 0,
// char code 69
        3, 6, 31, 21, 21, 0, 0,
// char code 70
        3, 6, 30, 5, 5, 0, 0,
// char code 71
        4, 6, 14, 17, 21, 29, 0,
// char code 72
        4, 6, 31, 4, 4, 31, 0,
// char code 73
        3, 6, 17, 31, 17, 0, 0,
// char code 74
        3, 6, 8, 17, 15, 0, 0,
// char code 75
        4, 6, 31, 4, 10, 17, 0,
// char code 76
        3, 6, 31, 16, 16, 0, 0,
// char code 77
        5, 6, 31, 2, 4, 2, 31,
// char code 78
        4, 6, 31, 2, 4, 31, 0,
// char code 79
        4, 6, 14, 17, 17, 14, 0,
// char code 80
        4, 6, 31, 9, 9, 6, 0,
// char code 81
        4, 6, 14, 17, 17, 46, 0,
// char code 82
        4, 6, 31, 9, 9, 22, 0,
// char code 83
        4, 6, 18, 21, 21, 9, 0,
// char code 84
        5, 6, 1, 1, 31, 1, 1,
// char code 85
        4, 6, 15, 16, 16, 15, 0,
// char code 86
        4, 6, 31, 8, 4, 3, 0,
// char code 87
        5, 6, 15, 16, 12, 16, 15,
// char code 88
        4, 6, 27, 4, 4, 27, 0,
// char code 89
        4, 6, 3, 20, 20, 15, 0,
// char code 90
        3, 6, 25, 21, 19, 0, 0,
// char code 91
        2, 6, 31, 17, 0, 0, 0,
// char code 92
        5, 6, 1, 2, 4, 8, 16,
// char code 93
        2, 6, 17, 31, 0, 0, 0,
// char code 94
        3, 6, 2, 1, 2, 0, 0,
// char code 95
        4, 6, 16, 16, 16, 16, 0,
// char code 96
        2, 6, 1, 2, 0, 0, 0,
// char code 97
        4, 6, 12, 18, 18, 30, 0,
// char code 98
        4, 6, 31, 18, 18, 12, 0,
// char code 99
        3, 6, 12, 18, 18, 0, 0,
// char code 100
        4, 6, 12, 18, 18, 15, 0,
// char code 101
        3, 6, 12, 26, 22, 0, 0,
// char code 102
        3, 6, 4, 30, 5, 0, 0,
// char code 103
        4, 6, 4, 42, 42, 30, 0,
// char code 104
        3, 6, 31, 4, 24, 0, 0,
// char code 105
        1, 6, 29, 0, 0, 0, 0,
// char code 106
        2, 6, 32, 61, 0, 0, 0,
// char code 107
        3, 6, 31, 8, 20, 0, 0,
// char code 108
        1, 6, 31, 0, 0, 0, 0,
// char code 109
        5, 6, 30, 2, 30, 2, 28,
// char code 110
        3, 6, 30, 2, 28, 0, 0,
// char code 111
        4, 6, 12, 18, 18, 12, 0,
// char code 112
        4, 6, 62, 18, 18, 12, 0,
// char code 113
        4, 6, 12, 18, 18, 60, 0,
// char code 114
        3, 6, 30, 4, 2, 0, 0,
// char code 115
        3, 6, 20, 22, 10, 0, 0,
// char code 116
        3, 6, 4, 31, 20, 0, 0,
// char code 117
        3, 6, 30, 16, 30, 0, 0,
// char code 118
        3, 6, 14, 16, 14, 0, 0,
// char code 119
        5, 6, 6, 24, 6, 24, 6,
// char code 120
        3, 6, 18, 12, 18, 0, 0,
// char code 121
        3, 6, 38, 40, 30, 0, 0,
// char code 122
        3, 6, 18, 26, 22, 0, 0,
// char code 123
        3, 6, 4, 27, 17, 0, 0,
// char code 124
        1, 6, 63, 0, 0, 0, 0,
// char code 125
        3, 6, 17, 27, 4, 0, 0,
// char code 126
        4, 6, 2, 1, 2, 1, 0,
// char code 127
        0, 6, 0, 0, 0, 0, 0,
};

static const FontDefinition def = {
        .firstCharCode = 32,
        .lastCharCode = 127,
        .width = 5,
        .height = 6,
        .horizontalStep = 1,
        .verticalStep = 1,
        .lineHeight = 1,
        .isMono = false,
        .data = data
};

const Font FONT_SMALL = &def;
