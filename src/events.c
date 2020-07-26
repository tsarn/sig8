#include "sig8_internal.h"

typedef struct {
    int type;
    union {
        EventCallback eventCallback;
        Callback callback;
    };
} EventHandler;

static EventHandler handlers[MAX_EVENT_HANDLERS];
static int numHandlers = 0;

void sig8_RegisterCallback(int type, Callback callback)
{
    assert(numHandlers < MAX_EVENT_HANDLERS);
#ifndef SIG8_COMPILE_EDITORS
    if (type != FRAME_EVENT) {
        return;
    }
#endif
    handlers[numHandlers++] = (EventHandler){
            .type = type,
            .callback = callback,
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

bool sig8_EmitEvent(int type, SDL_Event *event)
{
    bool flag = false;
    for (int i = 0; i < numHandlers; ++i) {
        if (handlers[i].type == type) {
            flag = true;
            if (type < 0) {
                handlers[i].callback();
            } else {
                handlers[i].eventCallback(event);
            }
        }
    }
    return flag;
}
