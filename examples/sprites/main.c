#include "sig8.h"
#include <math.h>
#include <stdlib.h>

SpriteSheet spriteSheet;

#define NUMBER_OF_SPRITES 500
#define SPEED_RANGE 50

typedef struct {
    float x, y;
    float vx, vy;
    int sprite;
} Object;

Object objects[NUMBER_OF_SPRITES];

void mainLoop(void)
{
    float t = GetDelta();
    ClearScreen(BLACK);

    for (int i = 0; i < NUMBER_OF_SPRITES; ++i) {
        DrawSprite(objects[i].x - 10, objects[i].y - 10, objects[i].sprite);

        objects[i].x += objects[i].vx * t;
        objects[i].y += objects[i].vy * t;

        objects[i].x = fmodf(objects[i].x + 148, 148);
        objects[i].y = fmodf(objects[i].y + 148, 148);
    }

    DrawString(2, 9, WHITE, Format("FPS: %.1f", 1.0f / t));
}

int main()
{
    Initialize("sig8 example: sprite benchmark");
    spriteSheet = LoadSpriteSheet("res://spritesheet.png");
    UseSpriteSheet(spriteSheet);

    for (int i = 0; i < NUMBER_OF_SPRITES; ++i) {
        objects[i].x = rand() % 148;
        objects[i].y = rand() % 148;
        objects[i].sprite = rand() % 4;

        objects[i].vx = rand() % (2 * SPEED_RANGE + 1) - SPEED_RANGE;
        objects[i].vy = rand() % (2 * SPEED_RANGE + 1) - SPEED_RANGE;
    }

    RunMainLoop(mainLoop);
    return 0;
}
