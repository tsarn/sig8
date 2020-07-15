#include "sig8_internal.h"

typedef enum {
    LAYOUT_OTHER,
    LAYOUT_VBOX,
    LAYOUT_HBOX,
} LayoutType;

typedef struct {
    LayoutType type;
    int separation;
    int offset;
} LayoutInfo;

LayoutInfo layoutStack[MAX_LAYOUT_NESTING];
int layoutStackSize = 0;

void ResetLayout(void)
{
    layoutStackSize = 0;
}

int GetSeparation(void)
{
    return layoutStack[layoutStackSize - 1].separation;
}

void BeginMargin(int top, int right, int bottom, int left)
{
    PushArea();
    int w = GetAreaWidth();
    int h = GetAreaHeight();
    DrawingArea(left, top, w - left - right, h - top - bottom);
    layoutStack[layoutStackSize++] = (LayoutInfo){
        .type = LAYOUT_OTHER
    };
}

void BeginVBox(int separation)
{
    PushArea();
    layoutStack[layoutStackSize++] = (LayoutInfo){
            .type = LAYOUT_VBOX,
            .separation = separation,
            .offset = 0
    };
}

void BeginHBox(int separation)
{
    PushArea();
    layoutStack[layoutStackSize++] = (LayoutInfo){
            .type = LAYOUT_HBOX,
            .separation = separation,
            .offset = 0
    };
}

void BeginItem(int size)
{
    PushArea();
    LayoutInfo layout = layoutStack[layoutStackSize - 1];

    layoutStack[layoutStackSize - 1].offset += size + layout.separation;
    layoutStack[layoutStackSize++] = (LayoutInfo){
            .type = LAYOUT_OTHER
    };

    if (layout.type == LAYOUT_VBOX) {
        if (size == -1) {
            size = GetAreaHeight() - layout.offset;
        }
        DrawingArea(0, layout.offset, GetAreaWidth(), size);
    }

    if (layout.type == LAYOUT_HBOX) {
        if (size == -1) {
            size = GetAreaWidth() - layout.offset;
        }
        DrawingArea(layout.offset, 0, size, GetAreaHeight());
    }
}

void EndLayout(void)
{
    PopArea();
    --layoutStackSize;
}

void ColorLayout(int color)
{
    FillRect(0, 0, GetAreaWidth(), GetAreaHeight(), color);
}

int EqualSize(int amount)
{
    int size;
    LayoutInfo layout = layoutStack[layoutStackSize - 1];
    if (layout.type == LAYOUT_VBOX) {
        size = GetAreaHeight();
    } else {
        size = GetAreaWidth();
    }

    return (size - layout.separation * (amount - 1)) / amount;
}

bool Clickable(MouseButton button)
{
    if (!MouseJustPressed(button)) {
        return false;
    }

    return (
        mousePosition.x >= area.x &&
        mousePosition.y >= area.y &&
        mousePosition.x < area.x + area.width &&
        mousePosition.y < area.y + area.height
    );
}
