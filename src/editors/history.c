#include "editors.h"

static HistoryItem curAction;
static History history;
const ManagedResource *sig8_Editing;

void sig8_HistoryClear(void)
{
    for (int i = 0; i < history.size; ++i) {
        free(history.data[i].data);
    }

    if (history.data) {
        free(history.data);
    }

    history.data = NULL;
    history.size = 0;
    history.cur = 0;
    history.capacity = 0;
}

static void HistoryPush(HistoryItem item)
{
    for (int i = history.cur; i < history.size; ++i) {
        free(history.data[i].data);
    }

    history.size = history.cur;

    if (history.size + 1 > history.capacity) {
        int newCap = 2 * (history.size + 1);
        history.data = realloc(history.data, newCap * sizeof(HistoryItem));
        history.capacity = newCap;
    }

    history.data[history.size++] = item;
    ++history.cur;
}

static bool HistoryCanUndo(void)
{
    return history.cur > 0;
}

static bool HistoryCanRedo(void)
{
    return history.cur < history.size;
}

static HistoryItem HistoryUndo(void)
{
    return history.data[--history.cur];
}

static HistoryItem HistoryRedo(void)
{
    return history.data[history.cur++];
}

void sig8_BeginUndoableAction(void)
{
    int size = sig8_Editing->size;
    curAction.data = TempAlloc(2 * size);
    memcpy(curAction.data, sig8_Editing->resource, size);
}

void sig8_EndUndoableAction(void)
{
    int size = sig8_Editing->size;
    bool anythingChanged = false;
    for (int i = 0; i < size; ++i) {
        curAction.data[i] ^= sig8_Editing->resource[i];
        if (curAction.data[i]) {
            anythingChanged = true;
        }
    }

    if (!anythingChanged) {
        return;
    }

    // very simple compression
    int last = curAction.data[0], len = 1, j = 0;
    for (int i = 1; i <= size; ++i) {
        if (i < size) {
            if (curAction.data[i] == last) {
                if (len < 255) {
                    ++len;
                    continue;
                }
            }
        }

        curAction.data[j++] = len;
        curAction.data[j++] = last;

        last = curAction.data[i];
        len = 1;
    }

    uint8_t *oldData = curAction.data;

    curAction.data = malloc(j);
    memcpy(curAction.data, oldData, j);
    HistoryPush(curAction);
    curAction.data = NULL;
}


static void ApplyUndo(HistoryItem historyItem)
{
    int size = sig8_Editing->size;
    int i = 0, j = 0;
    while (j < size) {
        int end = historyItem.data[i++] + j;
        uint8_t t = historyItem.data[i++];
        for (; j < end; ++j) {
            sig8_Editing->resource[j] ^= t;
        }
    }
}

void sig8_Undo(void)
{
    if (HistoryCanUndo()) {
        ApplyUndo(HistoryUndo());
    }
}

void sig8_Redo(void)
{
    if (HistoryCanRedo()) {
        ApplyUndo(HistoryRedo());
    }
}
