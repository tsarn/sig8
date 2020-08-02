#include <sig8.h>
#include "editors.h"

#define WINDOW_SIZE 16

static SoundLib soundLib;
static MusicLib musicLib;
static Palette palette;
static int selectedOctave;
static int selected;
static int selectedFragment;
static int selectedRow;
static int offset;

static void DrawChannels(void)
{
    char buf[8];

    Rect bigRect = {
            .x = 20,
            .y = 24,
            .width = 156,
            .height = 112
    };

    if (sig8_IsMouseOver(bigRect)) {
        SetCursorShape(CURSOR_HAND);
        Position position = GetMousePosition();

        if (MousePressed(MOUSE_LEFT)) {
            selectedRow = (position.y - bigRect.y) / 7 + offset;
        }
    }

    for (int j = 0; j < WINDOW_SIZE; ++j) {
        int idx = j + offset;
        sprintf(buf, "%02d", idx);
        DrawString(bigRect.x - 12, bigRect.y + 6 + 7 * j, (selectedRow == idx) ? WHITE : INDIGO, buf);
    }

    for (int i = 0; i < MUSIC_CHANNELS; ++i) {
        int pattern = musicLib[selected].fragments[selectedFragment][i];
        sig8_DrawNumberInput(30 + 40 * i, 14, &pattern);
        if (pattern < 0) pattern = 0;
        if (pattern > NUMBER_OF_PATTERNS) pattern = NUMBER_OF_PATTERNS;
        musicLib[selected].fragments[selectedFragment][i] = pattern;
        --pattern;

        Rect rect = {
                .x = 20 + 40 * i,
                .y = 24,
                .width = 36,
                .height = 112
        };

        sig8_StrokeRectR(sig8_AddBorder(rect, 1), WHITE);

        for (int j = 0; j < WINDOW_SIZE; ++j) {
            int idx = j + offset;
            Rect r = {
                    .x = rect.x,
                    .y = rect.y + 7 * j,
                    .width = rect.width,
                    .height = 7
            };

            if (idx == selectedRow) {
                sig8_FillRectR(r, WHITE);
            }

            if (pattern == -1) {
                DrawString(r.x + 5, r.y + 6, GRAY, "---");
                DrawString(r.x + 20, r.y + 6, GRAY, "--");
                continue;
            }

            int note = musicLib[selected].patterns[pattern].notes[idx].note;
            int instrument = musicLib[selected].patterns[pattern].notes[idx].instrument;

            if (note != STOP_NOTE) {
                char *name = "--";
                int octave = (note - C1) / 12 + 1;
                note -= (octave - 1) * 12;

                switch (note) {
                case C1:
                    name = "C.";
                    break;
                case C1S:
                    name = "C#";
                    break;
                case D1:
                    name = "D.";
                    break;
                case D1S:
                    name = "D#";
                    break;
                case E1:
                    name = "E.";
                    break;
                case F1:
                    name = "F.";
                    break;
                case F1S:
                    name = "F#";
                    break;
                case G1:
                    name = "G.";
                    break;
                case G1S:
                    name = "G#";
                    break;
                case A1:
                    name = "A.";
                    break;
                case A1S:
                    name = "A#";
                    break;
                case B1:
                    name = "B";
                    break;
                default:
                    name = "??";
                }
                sprintf(buf, "%s%d", name, octave);
                DrawString(r.x + 5, r.y + 6, DARK_RED, buf);
                sprintf(buf, "%02d", instrument);
                DrawString(r.x + 20, r.y + 6, DARK_GREEN, buf);
            } else {
                DrawString(r.x + 5, r.y + 6, DARK_RED, "---");
                DrawString(r.x + 20, r.y + 6, DARK_GREEN, "--");
            }
        }
    }
}

static void DrawTrackSelect(void)
{
    sig8_DrawNumberInput(4, 4, &selected);
    selected = Modulo(selected, MUSICLIB_SIZE);
}

static void DrawFragmentSelect(void)
{
    int x = 30;
    DrawString(x, 10, WHITE, "FRAG");

    sig8_DrawNumberInput(x + 20, 4, &selectedFragment);
    if (selectedFragment < 0) selectedFragment = 0;
    if (selectedFragment >= TRACK_LENGTH) selectedFragment = TRACK_LENGTH - 1;
}

static void DrawTempoSelect(void)
{
    int x = 77;
    DrawString(x, 10, WHITE, "TEMPO");

    int tempo = musicLib[selected].tempo;
    sig8_DrawNumberInput(x + 27, 4, &tempo);
    if (tempo < 4) tempo = 4;
    if (tempo > 30) tempo = 30;
    musicLib[selected].tempo = tempo;
}

static void MoveSelection(int delta)
{
    int newSelectedRow = selectedRow + delta;
    int newOffset = offset;

    if (newSelectedRow >= PATTERN_LENGTH) {
        newSelectedRow = PATTERN_LENGTH - 1;
    }

    if (newSelectedRow < 0) {
        newSelectedRow = 0;
    }

    if (newSelectedRow >= offset + WINDOW_SIZE) {
        newOffset = newSelectedRow - WINDOW_SIZE + 1;
    }

    if (newSelectedRow < offset) {
        newOffset = newSelectedRow;
    }

    if (newOffset >= PATTERN_LENGTH - WINDOW_SIZE + 1) {
        newOffset = PATTERN_LENGTH - WINDOW_SIZE;
    }

    if (newOffset < 0) {
        newOffset = 0;
    }

    selectedRow = newSelectedRow;
    offset = newOffset;
}

static void HandleInput(void)
{
    if (KeyJustPressed("Down")) {
        MoveSelection(1);
    }

    if (KeyJustPressed("Up")) {
        MoveSelection(-1);
    }

    if (KeyJustPressed("PageDown")) {
        MoveSelection(WINDOW_SIZE / 2);
    }

    if (KeyJustPressed("PageUp")) {
        MoveSelection(-WINDOW_SIZE / 2);
    }

    if (KeyJustPressed("Home")) {
        MoveSelection(-PATTERN_LENGTH);
    }

    if (KeyJustPressed("End")) {
        MoveSelection(PATTERN_LENGTH);
    }

    if (KeyJustPressed("Escape")) {
        SetCursorShape(CURSOR_ARROW);
        sig8_HistoryClear();
        sig8_LeaveEditor();
        UsePalette(palette);
        return;
    }
}

void sig8_MusicEditorInit(ManagedResource *what)
{
    sig8_Editing = what;
    musicLib = (MusicLib) what->resource;
    soundLib = GetCurrentSoundLib();
    palette = GetPalette();
    selectedOctave = 4;
    UsePalette(PALETTE_DEFAULT);
}

void sig8_MusicEditorTick(void)
{
    UseSpriteSheet(sig8_EDITORS_SPRITESHEET);
    SetCursorShape(CURSOR_ARROW);
    UseFont(FONT_SMALL);
    sig8_StatusLine = "";

    ClearScreen(BLACK);
    HandleInput();
    DrawTrackSelect();
    DrawFragmentSelect();
    DrawTempoSelect();
    DrawChannels();
}

