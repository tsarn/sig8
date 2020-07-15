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

void BeginUI(void)
{
    SetCursorShape(CURSOR_ARROW);
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
    SetArea(left, top, w - left - right, h - top - bottom);
    layoutStack[layoutStackSize++] = (LayoutInfo){
        .type = LAYOUT_OTHER
    };
}

void BeginCenter(int width, int height)
{
    int w = GetAreaWidth();
    int h = GetAreaHeight();

    int left = (w - width) / 2;
    if (left < 0) left = 0;

    int right = w - width - left;
    if (right < 0) right = 0;

    int top = (h - height) / 2;
    if (top < 0) top = 0;

    int bottom = h - height - top;
    if (bottom < 0) bottom = 0;

    BeginMargin(top, right, bottom, left);
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
        SetArea(0, layout.offset, GetAreaWidth(), size);
    }

    if (layout.type == LAYOUT_HBOX) {
        if (size == -1) {
            size = GetAreaWidth() - layout.offset;
        }
        SetArea(layout.offset, 0, size, GetAreaHeight());
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

bool Button(MouseButton button)
{
    bool isInside = (
            mousePosition.x >= area.x &&
            mousePosition.y >= area.y &&
            mousePosition.x < area.x + area.width &&
            mousePosition.y < area.y + area.height
    );

    if (isInside) {
        SetCursorShape(CURSOR_HAND);
    }

    if (button == MOUSE_HOVER) {
        return isInside;
    }

    return isInside && MouseJustPressed(button);
}
