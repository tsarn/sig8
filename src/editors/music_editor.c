#include <sig8.h>
#include "editors.h"

#define WINDOW_SIZE 16
#define CHANNEL 0

static SoundLib soundLib;
static MusicLib musicLib;
static Palette palette;
static int selected;
static int selectedOctave;
static int selectedFragment;
static int selectedRow;
static int selectedInstrument;
static int selectedChannel;
static int offset;
static int notePreviewDuration;
static int playingRow;

static void UpdateState(void)
{
    bool playing;
    int track, fragment, row;
    sig8_GetMusicState(&playing, &track, &fragment, &row);
    if (playing && track == selected && fragment == selectedFragment) {
        playingRow = row;
    } else {
        playingRow = -1;
    }
}

static void PlayNotePreview(void)
{
    int pattern = musicLib[selected].fragments[selectedFragment][selectedChannel] - 1;
    UseInstrument(soundLib[selectedInstrument].instrument, CHANNEL);
    PlayNote(musicLib[selected].patterns[pattern].notes[selectedRow].note, CHANNEL);
    notePreviewDuration = 6 * FRAME_RATE / musicLib[selected].tempo;
}

static void DrawChannels(void)
{
    char buf[8];

    Rect bigRect = {
            .x = 14,
            .y = 24,
            .width = 172,
            .height = 112
    };

    if (sig8_IsMouseOver(bigRect)) {
        SetCursorShape(CURSOR_HAND);
        Position position = GetMousePosition();

        if (MousePressed(MOUSE_LEFT)) {
            selectedRow = (position.y - bigRect.y) / 7 + offset;
            selectedChannel = (position.x - bigRect.x) / 44;
        }
    }

    for (int j = 0; j < WINDOW_SIZE; ++j) {
        int idx = j + offset;
        sprintf(buf, "%02d", idx);
        DrawString(bigRect.x - 12, bigRect.y + 6 + 7 * j, (selectedRow == idx) ? WHITE : INDIGO, buf);
    }

    for (int i = 0; i < MUSIC_CHANNELS; ++i) {
        int pattern = musicLib[selected].fragments[selectedFragment][i];
        sig8_DrawNumberInput(24 + 44 * i, 14, &pattern);
        if (pattern < 0) pattern = 0;
        if (pattern > NUMBER_OF_PATTERNS) pattern = NUMBER_OF_PATTERNS;
        musicLib[selected].fragments[selectedFragment][i] = pattern;
        --pattern;

        Rect rect = {
                .x = 14 + 44 * i,
                .y = 24,
                .width = 40,
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

            int noteColor = DARK_RED;
            int instrumentColor = DARK_GREEN;

            if (idx == selectedRow || idx == playingRow) {
                if (i == selectedChannel || idx == playingRow) {
                    sig8_FillRectR(r, WHITE);
                    noteColor = instrumentColor = GRAY;
                } else {
                    noteColor = instrumentColor = WHITE;
                }
            }

            if (pattern == -1) {
                DrawString(r.x + 5, r.y + 6, GRAY, "----");
                DrawString(r.x + 22, r.y + 6, GRAY, "--");
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
                    name = "C";
                    break;
                case C1S:
                    name = "C#";
                    break;
                case D1:
                    name = "D";
                    break;
                case D1S:
                    name = "D#";
                    break;
                case E1:
                    name = "E";
                    break;
                case F1:
                    name = "F";
                    break;
                case F1S:
                    name = "F#";
                    break;
                case G1:
                    name = "G";
                    break;
                case G1S:
                    name = "G#";
                    break;
                case A1:
                    name = "A";
                    break;
                case A1S:
                    name = "A#";
                    break;
                case B1:
                    name = "B";
                    break;
                default:
                    name = "?";
                }
                sprintf(buf, "%s%d", name, octave);
                DrawString(r.x + 5, r.y + 6, noteColor, buf);
                sprintf(buf, "%02d", instrument);
                DrawString(r.x + 22, r.y + 6, instrumentColor, buf);
            } else {
                DrawString(r.x + 5, r.y + 6, noteColor, "----");
                DrawString(r.x + 22, r.y + 6, instrumentColor, "--");
            }
        }
    }
}

static void DrawOctaveSelect(void)
{
    char buf[2];
    buf[1] = '\0';

    DrawString(4, SCREEN_HEIGHT - 2, WHITE, "OCT");

    for (int i = 1; i <= 7; ++i) {
        Rect rect = {
                .x = 23 + (i - 1) * 7,
                .y = SCREEN_HEIGHT - 7,
                .width = 5,
                .height = 6
        };

        buf[0] = (char) ('0' + i);

        DrawString(rect.x, rect.y + 5, (selectedOctave == i) ? WHITE : INDIGO, buf);

        if (sig8_IsMouseOver(rect)) {
            SetCursorShape(CURSOR_HAND);
            if (MouseJustPressed(MOUSE_LEFT)) {
                selectedOctave = i;
            }
        }
    }
}

static void DrawInstrumentSelect(void)
{
    DrawString(85, SCREEN_HEIGHT - 2, WHITE, "INS");
    sig8_DrawNumberInput(101, SCREEN_HEIGHT - 8, &selectedInstrument);
    if (selectedInstrument < 0) selectedInstrument = 0;
    if (selectedInstrument >= SOUNDLIB_SIZE) selectedInstrument = SOUNDLIB_SIZE - 1;
}

static void DrawTrackSelect(void)
{
    sig8_DrawNumberInput(4, 4, &selected);
    selected = Modulo(selected, MUSICLIB_SIZE);
}

static void DrawFragmentSelect(void)
{
    char buf[4];

    for (int i = 0; i < TRACK_LENGTH; ++i) {
        Rect rect = {
                .x = SCREEN_WIDTH - 11,
                .y = 24 + 7 * i,
                .width = 11,
                .height = 7
        };

        if (sig8_IsMouseOver(rect)) {
            SetCursorShape(CURSOR_HAND);

            if (MousePressed(MOUSE_LEFT)) {
                selectedFragment = i;
            }
        }

        sprintf(buf, "%02d", i);
        if (selectedFragment == i) {
            sig8_FillRectR(rect, WHITE);
        }
        DrawString(rect.x + 1, rect.y + 6, (selectedFragment == i) ? BLACK : INDIGO, buf);
    }
}

static void DrawTempoSelect(void)
{
    int x = 25;
    DrawString(x, 10, WHITE, "TEMPO");

    int tempo = musicLib[selected].tempo;
    sig8_DrawNumberInput(x + 28, 4, &tempo);
    if (tempo < 1) tempo = 1;
    if (tempo > 99) tempo = 99;
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

    if (KeyJustPressed("Right")) {
        selectedChannel = Modulo(selectedChannel + 1, MUSIC_CHANNELS);
    }

    if (KeyJustPressed("Left")) {
        selectedChannel = Modulo(selectedChannel - 1, MUSIC_CHANNELS);
    }

    if (KeyJustPressed("Space")) {
        bool isPlaying;
        sig8_GetMusicState(&isPlaying, NULL, NULL, NULL);
        if (isPlaying) {
            StopTrack();
        } else {
            PlayTrack(selected);
        }
    }

    int note = -1;

    if (KeyJustPressed("Z")) note = C4;
    if (KeyJustPressed("S")) note = C4S;
    if (KeyJustPressed("X")) note = D4;
    if (KeyJustPressed("D")) note = D4S;
    if (KeyJustPressed("C")) note = E4;
    if (KeyJustPressed("V")) note = F4;
    if (KeyJustPressed("G")) note = F4S;
    if (KeyJustPressed("B")) note = G4;
    if (KeyJustPressed("H")) note = G4S;
    if (KeyJustPressed("N")) note = A4;
    if (KeyJustPressed("J")) note = A4S;
    if (KeyJustPressed("M")) note = B4;
    if (KeyJustPressed("Delete")) note = STOP_NOTE;

    if (note != -1) {
        int pattern = musicLib[selected].fragments[selectedFragment][selectedChannel] - 1;
        if (pattern != -1) {
            if (note != STOP_NOTE) {
                note += (selectedOctave - 4) * 12;
            }
            musicLib[selected].patterns[pattern].notes[selectedRow].note = note;
            musicLib[selected].patterns[pattern].notes[selectedRow].instrument = selectedInstrument;
            if (note != STOP_NOTE) {
                PlayNotePreview();
            } else {
                ReleaseNote(CHANNEL);
            }
            MoveSelection(1);
        }
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
    if (notePreviewDuration > 0) {
        if (--notePreviewDuration == 0) {
            ReleaseNote(CHANNEL);
        }
    }

    UseSpriteSheet(sig8_EDITORS_SPRITESHEET);
    SetCursorShape(CURSOR_ARROW);
    UseFont(FONT_SMALL);
    sig8_StatusLine = "";

    ClearScreen(BLACK);
    HandleInput();
    UpdateState();
    DrawTrackSelect();
    DrawFragmentSelect();
    DrawTempoSelect();
    DrawChannels();
    DrawOctaveSelect();
    DrawInstrumentSelect();
}
