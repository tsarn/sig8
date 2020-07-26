#ifdef SIG8_COMPILE_EDITORS
#include "editors.h"

void sig8_HistoryClear(History *history)
{
    for (int i = 0; i < history->size; ++i) {
        free(history->data[i].data);
    }
    free(history->data);
}

void sig8_HistoryPush(History *history, HistoryItem item)
{
    for (int i = history->cur; i < history->size; ++i) {
        free(history->data[i].data);
    }

    history->size = history->cur;

    if (history->size + 1 > history->capacity) {
        int newCap = 2 * (history->size + 1);
        history->data = realloc(history->data, newCap * sizeof(HistoryItem));
        history->capacity = newCap;
    }

    history->data[history->size++] = item;
    ++history->cur;
}

bool sig8_HistoryCanUndo(History *history)
{
    return history->cur > 0;
}

bool sig8_HistoryCanRedo(History *history)
{
    return history->cur < history->size;
}

HistoryItem sig8_HistoryUndo(History *history)
{
    return history->data[--history->cur];
}

HistoryItem sig8_HistoryRedo(History *history)
{
    return history->data[history->cur++];
}

#endif
