#pragma once

#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT (SCREEN_WIDTH / 4 * 3)
#define DEFAULT_PIXEL_SIZE 4
#define DEFAULT_SCREEN_WIDTH (SCREEN_WIDTH * DEFAULT_PIXEL_SIZE)
#define DEFAULT_SCREEN_HEIGHT (SCREEN_HEIGHT * DEFAULT_PIXEL_SIZE)

#define WINDOW_TITLE "sig8"

#define N_COLORS 16

// PICO-8 color scheme, licensed under CC0

static const char *ColorNames[N_COLORS] = {
        "#000000",
        "#1D2B53",
        "#7E2553",
        "#008751",
        "#AB5236",
        "#5F574F",
        "#C2C3C7",
        "#FFF1E8",
        "#FF004D",
        "#FFA300",
        "#FFEC27",
        "#00E436",
        "#29ADFF",
        "#83769C",
        "#FF77A8",
        "#FFCCAA"
};

// These defines should only be used by user applications

#define BLACK 0
#define DARK_BLUE 1
#define DARK_PURPLE 2
#define DARK_GREEN 3
#define BROWN 4
#define DARK_GRAY 5
#define LIGHT_GRAY 6
#define WHITE 7
#define RED 8
#define ORANGE 9
#define YELLOW 10
#define GREEN 11
#define BLUE 12
#define INDIGO 13
#define PINK 14
#define PEACH 15
