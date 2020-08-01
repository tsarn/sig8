#include "editors.h"

#define DOT_SIZE 5
#define ENVELOPE_RANGE 16
#define CHANNEL 0

static Palette palette;
static SoundLib soundLib;
static int selected = 0;
static EnvelopeType selectedEnvelope = ENVELOPE_VOLUME;
static Note activeNote;

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

static void DrawSoundSelect(void)
{
    sig8_DrawNumberInput(SCREEN_WIDTH - 4 - DOT_SIZE * ENVELOPE_LENGTH, 4, &selected);
    selected = Modulo(selected, SOUNDLIB_SIZE);
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
    }

    if (note != activeNote) {
        activeNote = note;
        UseInstrument(soundLib[selected].instrument, CHANNEL);
        PlayNote(activeNote, CHANNEL);
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
                if ((int)selectedEnvelope > GetLastEnvelope()) {
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

static void DrawEnvelopeEditor(void)
{
    DrawEnvelopeButtons();

    Rect rect = {
            .x = SCREEN_WIDTH - 4 - DOT_SIZE * ENVELOPE_LENGTH,
            .y = 15,
            .width = DOT_SIZE * ENVELOPE_LENGTH,
            .height = DOT_SIZE * ENVELOPE_RANGE
    };

    if (sig8_IsMouseOver(rect)) {
        Position pos = GetMousePosition();
        SetCursorShape(CURSOR_HAND);
        pos.x = (pos.x - rect.x) / DOT_SIZE;
        pos.y = (pos.y - rect.y) / DOT_SIZE;
        if (MousePressed(MOUSE_LEFT)) {
            soundLib[selected].instrument.envelopes[selectedEnvelope].value[pos.x] = ENVELOPE_RANGE - 1 - pos.y;
        }
    }

    for (int i = 0; i < ENVELOPE_LENGTH; ++i) {
        int t = soundLib[selected].instrument.envelopes[selectedEnvelope].value[i];
        for (int j = 0; j < ENVELOPE_RANGE; ++j) {
            FillRect(
                    rect.x + i * DOT_SIZE, rect.y + j * DOT_SIZE,
                    DOT_SIZE - 1, DOT_SIZE - 1,
                    (ENVELOPE_RANGE - 1 - j <= t) ? RED : INDIGO
            );
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
    DrawWaveButtons();
    DrawEnvelopeEditor();
    HandleInput();
}
