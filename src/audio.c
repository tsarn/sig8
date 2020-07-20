#include "sig8_internal.h"
#include <threads.h>

static SDL_AudioDeviceID audioDevice;
static int sampleOffset;
static int frameCount;
static float masterVolume = 1.0f;

static AudioFrame soundQueue[BACKLOG_SIZE];
static int soundQueueBack; // we push here
static int soundQueueFront; // we pop from here
static mtx_t soundQueueMutex;

static Instrument instruments[SOUND_CHANNELS];
static Note playingNotes[SOUND_CHANNELS];
static int playingNotesSince[SOUND_CHANNELS];

static AudioFrame SILENCE = {
    .volume = 0.0f
};

void FinalizeAudio(void)
{
}

// Note: this function doesn't lock the mutex!
static int SoundQueueSize(void)
{
    return (soundQueueBack - soundQueueFront + BACKLOG_SIZE) % BACKLOG_SIZE;
}

static AudioFrame SoundQueuePop(void)
{
    mtx_lock(&soundQueueMutex);
    if (shouldQuit || soundQueueBack == soundQueueFront) {
        mtx_unlock(&soundQueueMutex);
        return SILENCE;
    }

    AudioFrame res = soundQueue[soundQueueFront];
    soundQueueFront = (soundQueueFront + 1) % BACKLOG_SIZE;

    mtx_unlock(&soundQueueMutex);

    return res;
}

// Note: this function doesn't lock the mutex!
static void SoundQueuePush(AudioFrame frame)
{
    soundQueue[soundQueueBack] = frame;
    soundQueueBack = (soundQueueBack + 1) % BACKLOG_SIZE;
}

static void AudioCallback(void *userData, uint8_t *byteStream, int byteLen)
{
    float *stream = (float*)byteStream;
    int len = byteLen / sizeof(float);
    static AudioFrame sound;
    static float freq[SOUND_CHANNELS];

    for (int i = 0; i < len; ++i) {
        if (sampleOffset % SAMPLES_PER_FRAME == 0) {
            sound = SoundQueuePop();
            for (int ch = 0; ch < SOUND_CHANNELS; ++ch) {
                if (sound.notes[ch] == STOP_NOTE) {
                    continue;
                }
                freq[ch] = GetNoteFrequency(sound.notes[ch]);
            }
        }

        float totalValue = 0.0f;

        for (int ch = 0; ch < SOUND_CHANNELS; ++ch) {
            if (sound.notes[ch] == STOP_NOTE) {
                continue;
            }

            float t = freq[ch] * sampleOffset / SAMPLE_RATE;
            t = fmodf(t, 1.0f);

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
            totalValue += value;
        }

        if (totalValue > 1.0f) {
            totalValue = 1.0f;
        }

        if (totalValue < -1.0f) {
            totalValue = -1.0f;
        }

        *stream++ = totalValue * sound.volume;
        ++sampleOffset;
    }

    // Keep it from getting too big
    // An audio click once per six hours is tolerable
    sampleOffset %= (1 << 30);
}

static void PopulateQueue(void)
{
    AudioFrame frame;
    frame.volume = masterVolume;
    for (int ch = 0; ch < SOUND_CHANNELS; ++ch) {
        frame.duration[ch] = frameCount - playingNotesSince[ch];
        frame.notes[ch] = playingNotes[ch];
        frame.instruments[ch] = instruments[ch];
    }

    while (SoundQueueSize() < PRELOAD_SOUNDS) {
        SoundQueuePush(frame);
        for (int ch = 0; ch < SOUND_CHANNELS; ++ch) {
            ++frame.duration[ch];
        }
    }
}

void AudioFrameCallback(void)
{
    mtx_lock(&soundQueueMutex);
    PopulateQueue();
    mtx_unlock(&soundQueueMutex);
    ++frameCount;
}

static void ResetQueue(void)
{
    mtx_lock(&soundQueueMutex);
    soundQueueFront = soundQueueBack = 0;
    PopulateQueue();
    mtx_unlock(&soundQueueMutex);
}

void InitializeAudio(void)
{
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

void SetInstrument(int channel, Instrument instrument)
{
    instruments[channel] = instrument;
    ResetQueue();
}

void PlayNote(int channel, Note note)
{
    playingNotesSince[channel] = frameCount;
    playingNotes[channel] = note;
    ResetQueue();
}
