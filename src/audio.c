#include "sig8_internal.h"
#include <threads.h>

static SDL_AudioDeviceID audioDevice;
static int frameCount;
static float masterVolume = 1.0f;

static AudioFrame soundQueueStorage[2][PRELOAD_SOUNDS];
static AudioFrame *soundQueue;
static int soundQueueSize;
static mtx_t soundQueueMutex;

static Instrument instruments[SOUND_CHANNELS];
static Note notes[SOUND_CHANNELS];
static bool isPlaying[SOUND_CHANNELS];
static int playingSince[SOUND_CHANNELS];

// Audio thread variables, do not touch from main!
static AudioFrame sound;
static float channelFreq[SOUND_CHANNELS];
static int volumeEnvelope[SOUND_CHANNELS];
static int pitchEnvelope[SOUND_CHANNELS];
static int arpeggioEnvelope[SOUND_CHANNELS];
static int sampleCount = 0;
static int sampleOffset[SOUND_CHANNELS];
static float phaseShift[SOUND_CHANNELS];

static AudioFrame SILENCE = {
    .volume = 0.0f
};

static int GetEnvelopeValue(const Envelope *envelope, int time, bool active)
{
    if (!active) {
        time += envelope->loopEnd;
    } else if (time >= envelope->loopBegin && envelope->loopEnd > 0) {
        time -= envelope->loopBegin;
        time %= (envelope->loopEnd - envelope->loopBegin);
        time += envelope->loopBegin;
    }

    if (time >= ENVELOPE_LENGTH) {
        return 0;
    }

    return envelope->value[time];
}

static AudioFrame SoundQueuePop(void)
{
    mtx_lock(&soundQueueMutex);
    if (shouldQuit || soundQueueSize == 0) {
        mtx_unlock(&soundQueueMutex);
        return SILENCE;
    }

    AudioFrame res = soundQueue[--soundQueueSize];

    mtx_unlock(&soundQueueMutex);

    return res;
}

static float getArgument(int channel)
{
    float pitch = (float)pitchEnvelope[channel];
    pitch += 16 * arpeggioEnvelope[channel];

    float fr = channelFreq[channel] * powf(2.0f, pitch / 12.0f / 16.0f);

    float t = fr * sampleOffset[channel] * 1.0f / SAMPLE_RATE;
    t += phaseShift[channel];
    t = fmodf(t, 1.0f);

    return t;
}

static void AudioCallback(void *userData, uint8_t *byteStream, int byteLen)
{
    float *stream = (float*)byteStream;
    int len = byteLen / sizeof(float);

    for (int i = 0; i < len; ++i) {
        if (sampleCount == SAMPLES_PER_FRAME) {
            sampleCount = 0;
            sound = SoundQueuePop();
            for (int ch = 0; ch < SOUND_CHANNELS; ++ch) {
                if (sound.notes[ch] == STOP_NOTE) {
                    continue;
                }


                bool newNote = sound.duration[ch] == 0 && sound.isPlaying[ch];
                float t1 = 0.0f, t2 = 0.0f;

                if (newNote) {
                    sampleOffset[ch] = 0;
                    phaseShift[ch] = 0;
                } else {
                    t1 = getArgument(ch);
                }

                channelFreq[ch] = GetNoteFrequency(sound.notes[ch]);

                volumeEnvelope[ch] = GetEnvelopeValue(
                        &sound.instruments[ch].envelopes[ENVELOPE_VOLUME],
                        sound.duration[ch],
                        sound.isPlaying[ch]
                );

                pitchEnvelope[ch] = GetEnvelopeValue(
                        &sound.instruments[ch].envelopes[ENVELOPE_PITCH],
                        sound.duration[ch],
                        sound.isPlaying[ch]
                );

                arpeggioEnvelope[ch] = GetEnvelopeValue(
                        &sound.instruments[ch].envelopes[ENVELOPE_ARPEGGIO],
                        sound.duration[ch],
                        sound.isPlaying[ch]
                );

                if (!newNote) {
                    t2 = getArgument(ch);
                    phaseShift[ch] += t1 - t2;
                }
            }
        }

        float totalValue = 0.0f;

        for (int ch = 0; ch < SOUND_CHANNELS; ++ch) {
            if (sound.notes[ch] == STOP_NOTE) {
                continue;
            }

            float t = getArgument(ch);
            float value = 0.0f;

            switch (sound.instruments[ch].wave) {
            case NOISE:
                value = 1.0f * rand() / RAND_MAX;
                break;

            case SQUARE_WAVE:
                value = (t > 0.5f) ? 1 : -1;
                break;

            case SAWTOOTH_WAVE:
                value = 2 * t - 1;
                break;

            case TRIANGLE_WAVE:
                value = (t < 0.5f) ? (4 * t - 1) : (3 - 4 * t);
                break;

            case SINE_WAVE:
                value = sinf(t * 2 * M_PI);
                break;

            default:
                break;
            }

            value *= sound.instruments[ch].volume;

            value *= (float)volumeEnvelope[ch] / 255.0f;
            totalValue += value;

            ++sampleOffset[ch];
        }


        if (totalValue > 1.0f) {
            totalValue = 1.0f;
        }

        if (totalValue < -1.0f) {
            totalValue = -1.0f;
        }

        *stream++ = totalValue * sound.volume;
        ++sampleCount;
    }
}

static void PopulateQueue(void)
{
    AudioFrame frame;
    frame.volume = masterVolume;
    for (int ch = 0; ch < SOUND_CHANNELS; ++ch) {
        if (!isPlaying[ch] && frameCount - playingSince[ch] >= ENVELOPE_LENGTH) {
            notes[ch] = STOP_NOTE;
        }

        frame.duration[ch] = frameCount - playingSince[ch];
        frame.notes[ch] = notes[ch];
        frame.isPlaying[ch] = isPlaying[ch];
        frame.instruments[ch] = instruments[ch];
    }

    AudioFrame *queue = soundQueueStorage[soundQueue == soundQueueStorage[0]];

    for (int idx = 0; idx < PRELOAD_SOUNDS; ++idx) {
        queue[PRELOAD_SOUNDS - 1 - idx] = frame;
        for (int ch = 0; ch < SOUND_CHANNELS; ++ch) {
            ++frame.duration[ch];
        }
    }

    // Publish the queue
    mtx_lock(&soundQueueMutex);
    soundQueue = queue;
    soundQueueSize = PRELOAD_SOUNDS;
    mtx_unlock(&soundQueueMutex);
}

void AudioFrameCallback(void)
{
    ++frameCount;
    PopulateQueue();
}

void InitializeAudio(void)
{
    soundQueue = soundQueueStorage[0];
    for (int i = 0; i < SOUND_CHANNELS; ++i) {
        SILENCE.notes[i] = STOP_NOTE;
    }

    mtx_init(&soundQueueMutex, mtx_plain);
    SDL_AudioSpec want, have;
    SDL_zero(want);

    want.freq = SAMPLE_RATE;
    want.channels = 1;
    want.samples = SAMPLE_SIZE;
    want.format = AUDIO_F32;
    want.callback = AudioCallback;

    audioDevice = SDL_OpenAudioDevice(NULL, 0, &want, &have, 0);
    if (audioDevice == 0) {
        fprintf(stderr, "Failed to obtain audio device\n");
        Finalize();
        exit(EXIT_FAILURE);
    }

    SDL_PauseAudioDevice(audioDevice, 0);
}

float GetNoteFrequency(Note note)
{
    return 440.0f * powf(2.0f, ((int)note - 49) / 12.0f);
}

Instrument NewInstrument(void)
{
    Instrument res;
    memset(&res, 0, sizeof res);

    res.volume = 1.0f;
    res.wave = SQUARE_WAVE;
    res.envelopes[ENVELOPE_VOLUME].loopEnd = 1;
    for (int i = 0; i < ENVELOPE_LENGTH; ++i) {
        res.envelopes[ENVELOPE_VOLUME].value[i] = (i == 0) ? 255 : 0;
        res.envelopes[ENVELOPE_DUTY_CYCLE].value[i] = 2;
    }

    return res;
}

void SetInstrument(int channel, Instrument instrument)
{
    instruments[channel] = instrument;
    PopulateQueue();
}

void PlayNote(int channel, Note note)
{
    if (note == STOP_NOTE) {
        StopNote(channel);
        return;
    }

    playingSince[channel] = frameCount;
    isPlaying[channel] = true;
    notes[channel] = note;
}

void StopNote(int channel)
{
    isPlaying[channel] = false;
    playingSince[channel] = frameCount;
}
