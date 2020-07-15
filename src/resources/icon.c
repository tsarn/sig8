#include "sig8_internal.h"

static const uint8_t iconData[] = {
        255, 255, 255, 255, 255, 255, 255,
        255, 8, 8, 255, 8, 8, 255,
        8, 8, 8, 8, 8, 7, 8,
        8, 8, 8, 8, 8, 8, 8,
        255, 8, 8, 8, 8, 8, 255,
        255, 255, 8, 8, 8, 255, 255,
        255, 255, 255, 8, 255, 255, 255,
};

static const SpriteDefinition iconDefinition = {
        .width = 7,
        .height = 7,
        .spriteData = iconData
};

const Sprite icon = &iconDefinition;
