#include "sig8_internal.h"

static Music playing;
static int offset;
static int loop;
static int waiting;
static Instrument instruments[256];

void PlayMusic(Music music)
{
    playing = music;
    offset = 0;
    loop = -1;
    waiting = 0;
}

void StopMusic(void)
{
    playing = NULL;
}

static int8_t GetByte(void)
{
    return (int8_t)playing[offset++];
}

void MusicFrameCallback(void)
{
    if (!playing) {
        return;
    }

    if (waiting > 0) {
        --waiting;
        return;
    }

    int instruction = GetByte();

    if (instruction == 0x01) {
        // wait
        waiting = GetByte();
    } else if (instruction == 0x02) {
        // instrument volume
        int idx = GetByte();
        instruments[idx].volume = (float)GetByte() / ENVELOPE_VOLUME_MAX;
    } else if (instruction == 0x03) {
        // instrument envelope
        int idx = GetByte();
        Envelope *env = &instruments[idx].envelopes[GetByte()];
        env->loopBegin = GetByte();
        env->loopEnd = GetByte();
        for (int i = 0; i < ENVELOPE_LENGTH; ++i) {
            env->value[i] = GetByte();
        }
    } else if (instruction == 0x04) {
        // instrument wave
        int idx = GetByte();
        instruments[idx].wave = GetByte();
    } else if (instruction == 0x05) {
        // channel instrument
        int channel = GetByte();
        int idx = GetByte();
        SetInstrument(channel, instruments[idx]);
    } else if (instruction == 0x06) {
        // play a note
        int channel = GetByte();
        int note = GetByte();
        PlayNote(channel, note);
    } else if (instruction == 0x07) {
        // loop here
        loop = offset;
    } else if (instruction == 0x08) {
        // end of song
        if (loop != -1) {
            offset = loop;
        } else {
            StopMusic();
        }
    }
}