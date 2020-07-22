#include "sig8_internal.h"

typedef struct {
    int type;
    union {
        EventCallback eventCallback;
        FrameCallback frameCallback;
    };
} EventHandler;

static EventHandler handlers[MAX_EVENT_HANDLERS];
static int numHandlers = 0;

void sig8_RegisterFrameCallback(FrameCallback callback)
{
    assert(numHandlers < MAX_EVENT_HANDLERS);
    handlers[numHandlers++] = (EventHandler){
            .type = FRAME_EVENT,
            .frameCallback = callback,
    };
}

void sig8_RegisterEventCallback(int type, EventCallback callback)
{
    assert(numHandlers < MAX_EVENT_HANDLERS);
    handlers[numHandlers++] = (EventHandler){
        .type = type,
        .eventCallback = callback
    };
}

void sig8_HandleEvent(int type, SDL_Event *event)
{
    for (int i = 0; i < numHandlers; ++i) {
        if (handlers[i].type == type) {
            if (type == FRAME_EVENT) {
                handlers[i].frameCallback();
            } else {
                handlers[i].eventCallback(event);
            }
        }
    }
}
