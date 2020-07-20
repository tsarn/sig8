#include "sig8_internal.h"
#include <threads.h>

static SDL_AudioDeviceID audioDevice;
static int sampleOffset;

static Sound soundQueue[BACKLOG_SIZE];
static int soundQueueBack; // we push here
static int soundQueueFront; // we pop from here
static mtx_t soundQueueMutex;
static cnd_t soundQueueCond; // condition variable when sound queue is not empty2

const Sound SILENCE = {
    .volume = 0.0f,
    .wave = SQUARE_WAVE,
    .note = A4
};

void FinalizeAudio(void)
{
    cnd_signal(&soundQueueCond);
}

static int SoundQueueSize(void)
{
    mtx_lock(&soundQueueMutex);
    int res = (soundQueueBack - soundQueueFront + BACKLOG_SIZE) % BACKLOG_SIZE;
    mtx_unlock(&soundQueueMutex);
    return res;
}

static Sound SoundQueuePop(void)
{
    if (shouldQuit) {
        return SILENCE;
    }

    mtx_lock(&soundQueueMutex);
    if (soundQueueBack == soundQueueFront) {
        mtx_unlock(&soundQueueMutex);
        return SILENCE;
    }

    Sound res = soundQueue[soundQueueFront];
    soundQueueFront = (soundQueueFront + 1) % BACKLOG_SIZE;

    mtx_unlock(&soundQueueMutex);

    return res;
}

static void SoundQueuePush(Sound sound)
{
    mtx_lock(&soundQueueMutex);
    soundQueue[soundQueueBack] = sound;
    soundQueueBack = (soundQueueBack + 1) % BACKLOG_SIZE;
    mtx_unlock(&soundQueueMutex);
    cnd_signal(&soundQueueCond);
}

static void AudioCallback(void *userData, uint8_t *byteStream, int byteLen)
{
    float *stream = (float*)byteStream;
    int len = byteLen / sizeof(float);
    static Sound sound;
    static float freq;

    for (int i = 0; i < len; ++i) {
        if (sampleOffset % SAMPLES_PER_FRAME == 0) {
            sound = SoundQueuePop();
            freq = GetNoteFrequency(sound.note);
        }

        float t = sampleOffset * freq / SAMPLE_RATE;
        t = fmodf(t, 1.0f);

        float value = 0.0f;

        switch (sound.wave) {
        case NOISE:
            value = 1.0f * rand() / RAND_MAX;
            break;

        case SQUARE_WAVE:
            value = (t > 0.5f) ? 1 : -1;
            break;

        case SAWTOOTH_WAVE:
            value = 2 * t - 1;
            break;

        case SINE_WAVE:
            value = sinf(t * 2 * M_PI);
            break;

        default:
            break;
        }

        *stream++ = value * sound.volume;
        ++sampleOffset;
    }

    // Keep it from getting too big
    // An audio click once per six hours is tolerable
    sampleOffset %= (1 << 30);
}

void AudioFrameCallback(void)
{
    while (SoundQueueSize() < PRELOAD_SOUNDS) {
        SoundQueuePush((Sound) {
                .volume = 0.5f,
                .wave = SINE_WAVE,
                .note = KeyPressed("Space") ? A5 : A4
        });
    }
}

void InitializeAudio(void)
{
    AudioFrameCallback();
    mtx_init(&soundQueueMutex, mtx_plain);
    cnd_init(&soundQueueCond);
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
