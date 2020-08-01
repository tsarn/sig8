#include "editors.h"

#define DOT_SIZE 5
#define ENVELOPE_RANGE 16
#define CHANNEL 0

static Palette palette;
static SoundLib soundLib;
static int selected = 0;
static EnvelopeType selectedEnvelope = ENVELOPE_VOLUME;
static Note activeNote;
static int selectedOctave;

static const char *waveNames[] = {
        "SQUARE WAVE",
        "SAWTOOTH WAVE",
        "SINE WAVE",
        "TRIANGLE WAVE",
        "NOISE"
};

static const char *envelopeNames[] = {
        "VOLUME",
        "PITCH",
        "R.PITCH",
        "ARPEGG",
        "DUTY",
};

static const Note whiteKeyNotes[] = {
        C4, D4, E4, F4, G4, A4, B4,
};

static const Note blackKeyNotes[] = {
        C4S, D4S, F4S, G4S, A4S
};

static int GetLastEnvelope(void)
{
    switch (soundLib[selected].instrument.wave) {
    case SQUARE_WAVE:
        return ENVELOPE_DUTY_CYCLE;

    case SAWTOOTH_WAVE:
    case SINE_WAVE:
    case TRIANGLE_WAVE:
        return ENVELOPE_ARPEGGIO;

    default:
        return ENVELOPE_VOLUME;
    }
}

static void DrawVolumeSlider(void)
{
    int t = soundLib[selected].instrument.volume;

    Rect rect = {
            .x = SCREEN_WIDTH - 4 - (ENVELOPE_VOLUME_MAX + 1) * DOT_SIZE,
            .y = SCREEN_HEIGHT - 40,
            .width = (ENVELOPE_VOLUME_MAX + 1) * DOT_SIZE,
            .height = DOT_SIZE
    };

    if (sig8_IsMouseOver(rect)) {
        Position pos = GetMousePosition();
        SetCursorShape(CURSOR_HAND);
        if (MousePressed(MOUSE_LEFT)) {
            soundLib[selected].instrument.volume = (pos.x - rect.x) / DOT_SIZE;
        }
    }

    DrawString(rect.x - MeasureString("VOLUME") - 3, rect.y + 4, WHITE, "VOLUME");

    for (int i = 0; i <= ENVELOPE_VOLUME_MAX; ++i) {
        FillRect(
                rect.x + i * DOT_SIZE, rect.y,
                DOT_SIZE - 1, DOT_SIZE - 1,
                (i <= t) ? RED : INDIGO
        );
    }
}

static void DrawOctaveSelect(void)
{
    char buf[4];
    buf[1] = '\0';

    for (int i = 1; i <= 7; ++i) {
        Rect rect = {
                .x = 8 + (i - 1) * 7,
                .y = SCREEN_HEIGHT - 41,
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

        if (KeyJustPressed(buf)) {
            selectedOctave = i;
        }
    }

    Note note = soundLib[selected].note;
    char *name;
    int octave = ((int) note - C1) / 12 + 1;
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
    DrawString(60, SCREEN_HEIGHT - 36, (activeNote != STOP_NOTE) ? WHITE : INDIGO, buf);
}

static void DrawSoundSelect(void)
{
    sig8_DrawNumberInput(SCREEN_WIDTH - 4 - DOT_SIZE * ENVELOPE_LENGTH, 4, &selected);
    selected = Modulo(selected, SOUNDLIB_SIZE);
}

static void DrawSpeedSelect(void)
{
    int x = SCREEN_WIDTH - 4 - DOT_SIZE * ENVELOPE_LENGTH + 30;
    DrawString(x, 10, WHITE, "SPEED");

    int speed = soundLib[selected].instrument.speed;
    sig8_DrawNumberInput(x + 25, 4, &speed);
    if (speed < 1) speed = 1;
    if (speed > 64) speed = 64;
    soundLib[selected].instrument.speed = speed;
}

static void DrawPiano(void)
{
    int x = 6;
    int y = SCREEN_HEIGHT - 30;
    Note whiteNote = STOP_NOTE;
    Note blackNote = STOP_NOTE;

    for (int i = 0; i < 7; ++i) {
        Note note = whiteKeyNotes[i];
        Rect rect = {
                .x = x + i * 10,
                .y = y,
                .width = 8,
                .height = 25
        };
        sig8_FillRectR(rect, (note == activeNote) ? RED : WHITE);

        rect.x -= 1;
        rect.width += 2;

        if (sig8_IsMouseOver(rect)) {
            if (MousePressed(MOUSE_LEFT)) {
                whiteNote = note;
            }
        }
    }

    for (int i = 0; i < 5; ++i) {
        Note note = blackKeyNotes[i];
        Rect rect = {
                .x = x + 6 + ((i >= 2) ? (i + 1) : i) * 10,
                .y = y,
                .width = 6,
                .height = 14
        };
        sig8_FillRectR(rect, (note == activeNote) ? RED : BLACK);

        if (sig8_IsMouseOver(rect)) {
            if (MousePressed(MOUSE_LEFT)) {
                blackNote = note;
            }
        }
    }

    Note note = (blackNote == STOP_NOTE) ? whiteNote : blackNote;

    if (note == STOP_NOTE) {
        if (KeyPressed("Z")) note = C4;
        if (KeyPressed("S")) note = C4S;
        if (KeyPressed("X")) note = D4;
        if (KeyPressed("D")) note = D4S;
        if (KeyPressed("C")) note = E4;
        if (KeyPressed("V")) note = F4;
        if (KeyPressed("G")) note = F4S;
        if (KeyPressed("B")) note = G4;
        if (KeyPressed("H")) note = G4S;
        if (KeyPressed("N")) note = A4;
        if (KeyPressed("J")) note = A4S;
        if (KeyPressed("M")) note = B4;

        if (KeyPressed("Space")) {
            note = Modulo((int) soundLib[selected].note - C4, 12) + C4;
            selectedOctave = Divide((int) soundLib[selected].note - C4, 12) + 4;
        }
    }

    if (note != activeNote) {
        activeNote = note;
        UseInstrument(soundLib[selected].instrument, CHANNEL);
        Note noteToPlay = activeNote;
        if (noteToPlay != STOP_NOTE) {
            noteToPlay += (selectedOctave - 4) * 12;
            soundLib[selected].note = noteToPlay;
        }
        PlayNote(noteToPlay, CHANNEL);
    }
}

static void DrawWaveButtons(void)
{
    for (int i = 0; i < NUMBER_OF_WAVES; ++i) {
        Rect rect = {
                .x = SCREEN_WIDTH - 80 + 15 * i,
                .y = 3,
                .width = 12,
                .height = 8
        };

        if (soundLib[selected].instrument.wave == (Wave) i) {
            RemapColor(WHITE, RED);
        }

        DrawBigSprite(rect.x, rect.y, 25, 2, 1);
        DrawSprite(rect.x + 2, rect.y, 20 + i);

        ResetColors();

        if (sig8_IsMouseOver(rect)) {
            SetCursorShape(CURSOR_HAND);

            if (MouseJustPressed(MOUSE_LEFT)) {
                soundLib[selected].instrument.wave = (Wave) i;
                if ((int) selectedEnvelope > GetLastEnvelope()) {
                    selectedEnvelope = ENVELOPE_VOLUME;
                }
            }
        }
    }
}

static void DrawEnvelopeButtons(void)
{
    int lastEnvelope = GetLastEnvelope();

    for (int i = 0; i <= lastEnvelope; ++i) {
        Rect rect = {
                .x = 4,
                .y = 15 + 10 * i,
                .width = MeasureString(envelopeNames[i]),
                .height = 8
        };

        DrawString(rect.x, rect.y + 5,
                   ((int) selectedEnvelope == i) ? RED : INDIGO,
                   envelopeNames[i]);

        if (sig8_IsMouseOver(rect)) {
            SetCursorShape(CURSOR_HAND);

            if (MouseJustPressed(MOUSE_LEFT)) {
                selectedEnvelope = (EnvelopeType) i;
            }
        }
    }
}

static int GetEnvelopeOrigin(EnvelopeType envelope)
{
    if (envelope == ENVELOPE_PITCH || envelope == ENVELOPE_REL_PITCH || envelope == ENVELOPE_ARPEGGIO) {
        return (ENVELOPE_RANGE + 1) / 2;
    } else {
        return 0;
    }
}

static void DrawEnvelopeEditor(void)
{
    DrawEnvelopeButtons();

    Rect rect = {
            .x = SCREEN_WIDTH - 4 - DOT_SIZE * ENVELOPE_LENGTH,
            .y = 15,
            .width = DOT_SIZE * ENVELOPE_LENGTH,
            .height = DOT_SIZE * ENVELOPE_RANGE
    };

    Envelope *envelope = &soundLib[selected].instrument.envelopes[selectedEnvelope];
    int time = sig8_GetPlayingTime(CHANNEL, envelope);

    if (time >= 0 && time < ENVELOPE_LENGTH) {
        FillRect(rect.x + time * DOT_SIZE - 1, rect.y - 1, DOT_SIZE + 1, rect.height + 1, WHITE);
    }

    int origin = GetEnvelopeOrigin(selectedEnvelope);

    if (sig8_IsMouseOver(rect)) {
        Position pos = GetMousePosition();
        SetCursorShape(CURSOR_HAND);
        pos.x = (pos.x - rect.x) / DOT_SIZE;
        pos.y = (pos.y - rect.y) / DOT_SIZE;
        int val = ENVELOPE_RANGE - 1 - pos.y - origin;
        if (MousePressed(MOUSE_LEFT)) {
            soundLib[selected].instrument.envelopes[selectedEnvelope].value[pos.x] = val;
        } else if (MousePressed(MOUSE_RIGHT)) {
            for (int i = 0; i < ENVELOPE_LENGTH; ++i) {
                soundLib[selected].instrument.envelopes[selectedEnvelope].value[i] = val;
            }
        }
    }

    for (int i = 0; i < ENVELOPE_LENGTH; ++i) {
        int t = soundLib[selected].instrument.envelopes[selectedEnvelope].value[i];
        for (int j = 0; j < ENVELOPE_RANGE; ++j) {
            int val = ENVELOPE_RANGE - 1 - j - origin;
            FillRect(
                    rect.x + i * DOT_SIZE, rect.y + j * DOT_SIZE,
                    DOT_SIZE - 1, DOT_SIZE - 1,
                    ((t > 0) ? (val >= 0 && val <= t) : (val <= 0 && val >= t)) ? RED : INDIGO
            );
        }
    }
}

static void DrawLoopEditor(void)
{
    Rect rect = {
            .x = SCREEN_WIDTH - 4 - DOT_SIZE * ENVELOPE_LENGTH,
            .y = 20 + ENVELOPE_RANGE * DOT_SIZE,
            .width = DOT_SIZE * ENVELOPE_LENGTH,
            .height = DOT_SIZE - 1
    };

    int8_t *begin = &soundLib[selected].instrument.envelopes[selectedEnvelope].loopBegin;
    int8_t *end = &soundLib[selected].instrument.envelopes[selectedEnvelope].loopEnd;

    if (sig8_IsMouseOver(rect)) {
        Position pos = GetMousePosition();
        SetCursorShape(CURSOR_HAND);

        int t = (pos.x - rect.x) / DOT_SIZE;

        if (MousePressed(MOUSE_RIGHT)) {
            if (t > *end) {
                *end = t;
            } else {
                *begin = t;
            }
        }

        if (MousePressed(MOUSE_LEFT)) {
            if (t < *begin) {
                *begin = t;
            } else {
                *end = t;
            }
        }
    }

    DrawString(4, rect.y + 4, WHITE, "LOOP");

    for (int i = 0; i < ENVELOPE_LENGTH; ++i) {
        FillRect(
                rect.x + i * DOT_SIZE, rect.y,
                DOT_SIZE - 1, DOT_SIZE - 1,
                INDIGO
        );

        if (i == *begin) {
            sig8_DrawIcon(rect.x + i * DOT_SIZE, rect.y, 29, GREEN);
        }

        if (i == *end) {
            sig8_DrawIcon(rect.x + i * DOT_SIZE, rect.y, 30, GREEN);
        }
    }
}

static void HandleInput(void)
{
    if (KeyJustPressed("Escape")) {
        SetCursorShape(CURSOR_ARROW);
        sig8_HistoryClear();
        sig8_LeaveEditor();
        UsePalette(palette);
        return;
    }
}

void sig8_SoundEditorInit(ManagedResource *what)
{
    sig8_Editing = what;
    soundLib = (SoundLib) what->resource;
    palette = GetPalette();
    activeNote = STOP_NOTE;
    selectedOctave = 4;
    UsePalette(PALETTE_DEFAULT);
    UseSoundLib(soundLib);
}

void sig8_SoundEditorTick(void)
{
    UseSpriteSheet(sig8_EDITORS_SPRITESHEET);
    SetCursorShape(CURSOR_ARROW);
    UseFont(FONT_SMALL);
    ClearScreen(BLACK);
    DrawPiano();
    DrawSoundSelect();
    DrawSpeedSelect();
    DrawVolumeSlider();
    DrawOctaveSelect();
    DrawWaveButtons();
    DrawEnvelopeEditor();
    DrawLoopEditor();
    HandleInput();
}
