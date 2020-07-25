#include "sig8tk.h"

void HistoryPush(History *history, HistoryItem item)
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

bool HistoryCanUndo(History *history)
{
    return history->cur > 0;
}

bool HistoryCanRedo(History *history)
{
    return history->cur < history->size;
}

HistoryItem HistoryUndo(History *history)
{
    return history->data[--history->cur];
}

HistoryItem HistoryRedo(History *history)
{
    return history->data[history->cur++];
}
