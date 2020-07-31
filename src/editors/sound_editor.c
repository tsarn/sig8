#include "editors.h"

#define DOT_SIZE 5
#define ENVELOPE_RANGE 16

static Palette palette;
static SoundLib soundLib;
static int selected = 0;
static EnvelopeType selectedEnvelope = ENVELOPE_VOLUME;

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
            }
        }
    }
}

static void DrawEnvelopeButtons(void)
{
    int lastEnvelope;

    switch (soundLib[selected].instrument.wave) {
    case SQUARE_WAVE:
        lastEnvelope = ENVELOPE_DUTY_CYCLE;
        break;

    case SAWTOOTH_WAVE:
    case SINE_WAVE:
    case TRIANGLE_WAVE:
        lastEnvelope = ENVELOPE_ARPEGGIO;
        break;

    default:
        lastEnvelope = ENVELOPE_VOLUME;
    }

    for (int i = 0; i <= lastEnvelope; ++i) {
        Rect rect = {
            .x = 4,
            .y = 15 + 10 * i,
            .width = MeasureString(envelopeNames[i]),
            .height = 8
        };

        DrawString(rect.x, rect.y + 5,
                ((int)selectedEnvelope == i) ? RED : INDIGO,
                envelopeNames[i]);

        if (sig8_IsMouseOver(rect)) {
            SetCursorShape(CURSOR_HAND);

            if (MouseJustPressed(MOUSE_LEFT)) {
                selectedEnvelope = (EnvelopeType)i;
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
    UsePalette(PALETTE_DEFAULT);
    UseSoundLib(soundLib);
}

void sig8_SoundEditorTick(void)
{
    UseSpriteSheet(sig8_EDITORS_SPRITESHEET);
    SetCursorShape(CURSOR_ARROW);
    UseFont(FONT_SMALL);
    ClearScreen(BLACK);
    DrawWaveButtons();
    DrawEnvelopeEditor();
    HandleInput();
}
