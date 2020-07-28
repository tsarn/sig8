#include "sig8.h"
#include <math.h>
#include <stdlib.h>

SpriteSheet spriteSheet;

#define NUMBER_OF_SPRITES 200000
#define SCREEN_OFFSET 10
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
        DrawSprite(objects[i].x, objects[i].y, objects[i].sprite);
        objects[i].x = fmod((t * objects[i].vx + objects[i].x + SCREEN_OFFSET), (SCREEN_WIDTH + 2 * SCREEN_OFFSET)) - SCREEN_OFFSET;
        objects[i].y = fmod((t * objects[i].vy + objects[i].y + SCREEN_OFFSET), (SCREEN_HEIGHT + 2 * SCREEN_OFFSET)) - SCREEN_OFFSET;
    }

    FillRect(0, 0, 128, 10, BLACK);
    DrawString(2, 9, WHITE, Format("FPS: %.1f", 1.0f / t));
}

int main()
{
    Initialize("sig8 example: sprite benchmark");
    spriteSheet = LoadSpriteSheet("res://spritesheet.png");
    UseSpriteSheet(spriteSheet);

    for (int i = 0; i < NUMBER_OF_SPRITES; ++i) {
        objects[i].x = rand() % (SCREEN_WIDTH + 2 * SCREEN_OFFSET) - SCREEN_OFFSET;
        objects[i].y = rand() % (SCREEN_HEIGHT + 2 * SCREEN_OFFSET) - SCREEN_OFFSET;
        objects[i].sprite = rand() % 4;

        objects[i].vx = rand() % (2 * SPEED_RANGE + 1) - SPEED_RANGE;
        objects[i].vy = rand() % (2 * SPEED_RANGE + 1) - SPEED_RANGE;
    }

    RunMainLoop(mainLoop);
    return 0;
}
