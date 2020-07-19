#include "sig8_internal.h"

/*
 * Aseprite Font, as seen here:
 * https://github.com/aseprite/aseprite/tree/master/data/fonts
 *
 * Created by David Capello,
 * licensed under Creative Commons Attribution 4.0 International License.
 */

static const uint8_t data[] = {
// char code 32
        4, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0,
// char code 33
        2, 7, 47, 0, 0, 0, 0, 0, 0, 0, 0,
// char code 34
        4, 7, 7, 0, 7, 0, 0, 0, 0, 0, 0,
// char code 35
        6, 7, 20, 62, 20, 62, 20, 0, 0, 0, 0,
// char code 36
        5, 7, 36, 42, 107, 18, 0, 0, 0, 0, 0,
// char code 37
        6, 7, 38, 22, 8, 52, 50, 0, 0, 0, 0,
// char code 38
        6, 7, 54, 73, 86, 32, 80, 0, 0, 0, 0,
// char code 39
        3, 7, 4, 3, 0, 0, 0, 0, 0, 0, 0,
// char code 40
        3, 7, 62, 65, 0, 0, 0, 0, 0, 0, 0,
// char code 41
        3, 7, 65, 62, 0, 0, 0, 0, 0, 0, 0,
// char code 42
        6, 7, 8, 42, 28, 42, 8, 0, 0, 0, 0,
// char code 43
        6, 7, 8, 8, 62, 8, 8, 0, 0, 0, 0,
// char code 44
        2, 7, 64, 48, 0, 0, 0, 0, 0, 0, 0,
// char code 45
        4, 7, 8, 8, 8, 0, 0, 0, 0, 0, 0,
// char code 46
        2, 7, 32, 0, 0, 0, 0, 0, 0, 0, 0,
// char code 47
        4, 7, 96, 28, 3, 0, 0, 0, 0, 0, 0,
// char code 48
        5, 7, 30, 33, 33, 30, 0, 0, 0, 0, 0,
// char code 49
        3, 7, 2, 63, 0, 0, 0, 0, 0, 0, 0,
// char code 50
        5, 7, 49, 41, 41, 38, 0, 0, 0, 0, 0,
// char code 51
        5, 7, 33, 37, 37, 26, 0, 0, 0, 0, 0,
// char code 52
        5, 7, 12, 10, 61, 8, 0, 0, 0, 0, 0,
// char code 53
        5, 7, 39, 37, 37, 24, 0, 0, 0, 0, 0,
// char code 54
        5, 7, 30, 37, 37, 24, 0, 0, 0, 0, 0,
// char code 55
        5, 7, 1, 49, 13, 3, 0, 0, 0, 0, 0,
// char code 56
        5, 7, 26, 37, 37, 26, 0, 0, 0, 0, 0,
// char code 57
        5, 7, 6, 41, 41, 30, 0, 0, 0, 0, 0,
// char code 58
        2, 7, 34, 0, 0, 0, 0, 0, 0, 0, 0,
// char code 59
        2, 7, 64, 50, 0, 0, 0, 0, 0, 0, 0,
// char code 60
        4, 7, 8, 20, 34, 0, 0, 0, 0, 0, 0,
// char code 61
        5, 7, 20, 20, 20, 20, 0, 0, 0, 0, 0,
// char code 62
        4, 7, 34, 20, 8, 0, 0, 0, 0, 0, 0,
// char code 63
        4, 7, 2, 1, 41, 6, 0, 0, 0, 0, 0,
// char code 64
        9, 7, 60, 66, 25, 37, 37, 121, 66, 60, 0,
// char code 65
        5, 7, 62, 9, 9, 62, 0, 0, 0, 0, 0,
// char code 66
        5, 7, 63, 37, 37, 26, 0, 0, 0, 0, 0,
// char code 67
        5, 7, 30, 33, 33, 33, 0, 0, 0, 0, 0,
// char code 68
        5, 7, 63, 33, 33, 30, 0, 0, 0, 0, 0,
// char code 69
        5, 7, 63, 37, 37, 33, 0, 0, 0, 0, 0,
// char code 70
        5, 7, 63, 5, 5, 1, 0, 0, 0, 0, 0,
// char code 71
        5, 7, 30, 33, 37, 61, 0, 0, 0, 0, 0,
// char code 72
        5, 7, 63, 4, 4, 63, 0, 0, 0, 0, 0,
// char code 73
        2, 7, 63, 0, 0, 0, 0, 0, 0, 0, 0,
// char code 74
        3, 7, 32, 31, 0, 0, 0, 0, 0, 0, 0,
// char code 75
        5, 7, 63, 12, 18, 33, 0, 0, 0, 0, 0,
// char code 76
        5, 7, 63, 32, 32, 32, 0, 0, 0, 0, 0,
// char code 77
        6, 7, 63, 2, 4, 2, 63, 0, 0, 0, 0,
// char code 78
        5, 7, 63, 2, 4, 63, 0, 0, 0, 0, 0,
// char code 79
        6, 7, 30, 33, 33, 33, 30, 0, 0, 0, 0,
// char code 80
        5, 7, 63, 9, 9, 6, 0, 0, 0, 0, 0,
// char code 81
        6, 7, 30, 33, 33, 97, 94, 0, 0, 0, 0,
// char code 82
        5, 7, 63, 9, 9, 54, 0, 0, 0, 0, 0,
// char code 83
        5, 7, 34, 37, 37, 25, 0, 0, 0, 0, 0,
// char code 84
        6, 7, 1, 1, 63, 1, 1, 0, 0, 0, 0,
// char code 85
        5, 7, 31, 32, 32, 31, 0, 0, 0, 0, 0,
// char code 86
        6, 7, 15, 16, 32, 16, 15, 0, 0, 0, 0,
// char code 87
        8, 7, 15, 16, 32, 28, 32, 16, 15, 0, 0,
// char code 88
        6, 7, 33, 18, 12, 18, 33, 0, 0, 0, 0,
// char code 89
        6, 7, 3, 4, 56, 4, 3, 0, 0, 0, 0,
// char code 90
        5, 7, 49, 41, 37, 35, 0, 0, 0, 0, 0,
// char code 91
        3, 7, 127, 65, 0, 0, 0, 0, 0, 0, 0,
// char code 92
        4, 7, 3, 28, 96, 0, 0, 0, 0, 0, 0,
// char code 93
        3, 7, 65, 127, 0, 0, 0, 0, 0, 0, 0,
// char code 94
        6, 7, 4, 2, 1, 2, 4, 0, 0, 0, 0,
// char code 95
        6, 7, 64, 64, 64, 64, 64, 0, 0, 0, 0,
// char code 96
        4, 7, 1, 2, 4, 0, 0, 0, 0, 0, 0,
// char code 97
        5, 7, 16, 42, 42, 60, 0, 0, 0, 0, 0,
// char code 98
        5, 7, 63, 36, 36, 24, 0, 0, 0, 0, 0,
// char code 99
        5, 7, 28, 34, 34, 34, 0, 0, 0, 0, 0,
// char code 100
        5, 7, 24, 36, 36, 63, 0, 0, 0, 0, 0,
// char code 101
        5, 7, 28, 42, 42, 12, 0, 0, 0, 0, 0,
// char code 102
        4, 7, 0, 62, 5, 0, 0, 0, 0, 0, 0,
// char code 103
        5, 7, 12, 82, 82, 60, 0, 0, 0, 0, 0,
// char code 104
        5, 7, 63, 4, 4, 56, 0, 0, 0, 0, 0,
// char code 105
        2, 7, 61, 0, 0, 0, 0, 0, 0, 0, 0,
// char code 106
        3, 7, 64, 61, 0, 0, 0, 0, 0, 0, 0,
// char code 107
        5, 7, 63, 8, 20, 34, 0, 0, 0, 0, 0,
// char code 108
        2, 7, 63, 0, 0, 0, 0, 0, 0, 0, 0,
// char code 109
        8, 7, 62, 2, 2, 62, 2, 2, 60, 0, 0,
// char code 110
        5, 7, 62, 2, 2, 60, 0, 0, 0, 0, 0,
// char code 111
        5, 7, 28, 34, 34, 28, 0, 0, 0, 0, 0,
// char code 112
        5, 7, 126, 18, 18, 12, 0, 0, 0, 0, 0,
// char code 113
        5, 7, 12, 18, 18, 126, 0, 0, 0, 0, 0,
// char code 114
        4, 7, 62, 4, 2, 0, 0, 0, 0, 0, 0,
// char code 115
        4, 7, 36, 42, 18, 0, 0, 0, 0, 0, 0,
// char code 116
        3, 7, 31, 34, 0, 0, 0, 0, 0, 0, 0,
// char code 117
        5, 7, 30, 32, 32, 62, 0, 0, 0, 0, 0,
// char code 118
        5, 7, 14, 16, 32, 30, 0, 0, 0, 0, 0,
// char code 119
        6, 7, 30, 32, 24, 32, 30, 0, 0, 0, 0,
// char code 120
        6, 7, 34, 20, 8, 20, 34, 0, 0, 0, 0,
// char code 121
        5, 7, 14, 80, 80, 62, 0, 0, 0, 0, 0,
// char code 122
        5, 7, 50, 42, 38, 34, 0, 0, 0, 0, 0,
// char code 123
        4, 7, 8, 54, 65, 0, 0, 0, 0, 0, 0,
// char code 124
        2, 7, 127, 0, 0, 0, 0, 0, 0, 0, 0,
// char code 125
        4, 7, 65, 54, 8, 0, 0, 0, 0, 0, 0,
// char code 126
        5, 7, 2, 1, 2, 1, 0, 0, 0, 0, 0,
// char code 127
        6, 7, 56, 36, 34, 36, 56, 0, 0, 0, 0,
};

static const FontDefinition def = {
        .firstCharCode = 32,
        .lastCharCode = 127,
        .width = 9,
        .height = 8,
        .horizontalStep = 0,
        .verticalStep = 0,
        .lineHeight = 1,
        .isMono = false,
        .data = data
};

const Font FONT_ASEPRITE = &def;
