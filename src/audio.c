#include "sig8_internal.h"

#define EXP_BASE 10.0f

static SDL_AudioDeviceID audioDevice;
static int curFrame; // frame count since start of the program
static float masterVolume = 1.0f;

typedef struct {
    Instrument instrument;
    int playingSince;
    int stoppedSince;
    float volume;
    bool isPlaying;
    Note note;
} ChannelState;

static ChannelState channels[SOUND_CHANNELS];

// Audio thread uses AudioFrame to generate audio
// for one frame (1 / 60th of a second).
// It doesn't use info from 'channels' array because
// it's not thread safe to do so: we don't want to change
// what note is playing in the middle of the frame
typedef struct {
    ChannelState channels[SOUND_CHANNELS];
    int time;
    float volume;
} AudioFrame;

// Sound queue, two of them, so when we need to write to it,
// we only lock the mutex to swap the soundQueue pointer
static AudioFrame soundQueueStorage[2][PRELOAD_SOUNDS];
static AudioFrame *soundQueue;
static int soundQueueSize;
static SDL_mutex *soundQueueMutex;

// Audio thread variables, do not touch from other threads.

// Current AudioFrame used to generate sound
static AudioFrame audioFrame;

// Current sample index in the current frame
static int curSampleIdx;

// Base frequency of currently playing note
static float baseFrequency[SOUND_CHANNELS];

// Resets when a new note starts playing
static int samplesSinceStart[SOUND_CHANNELS];

// How much to offset the wave, used to fix clicking when changing pitch
static float phaseShift[SOUND_CHANNELS];

// Current values of envelopes
static int curEnvelope[SOUND_CHANNELS][NUMBER_OF_ENVELOPES];

// Next frame values of envelopes, so we can interpolate
static int nextEnvelope[SOUND_CHANNELS][NUMBER_OF_ENVELOPES];

static AudioFrame SILENCE = {
    .volume = 0.0f
};

static int GetPlayingTime(const Envelope *envelope, int time, int stopTime, bool active)
{
    if (!active) {
        time -= stopTime;
    }

    if (time >= envelope->loopBegin && envelope->loopEnd > envelope->loopBegin) {
        time -= envelope->loopBegin;
        time %= (envelope->loopEnd - envelope->loopBegin);
        time += envelope->loopBegin;
    }

    if (!active) {
        time += stopTime;
    }

    return time;
}

int sig8_GetPlayingTime(int channel, const Envelope *envelope)
{
    if (channels[channel].note == STOP_NOTE) {
        return -1;
    }

    int time = (curFrame - channels[channel].playingSince) / channels[channel].instrument.speed;
    int stopTime = (curFrame - channels[channel].stoppedSince) / channels[channel].instrument.speed;

    return GetPlayingTime(envelope, time, stopTime, channels[channel].isPlaying);
}

static float Volume(float x)
{
    if (x < 0.0f) x = 0.0f;
    if (x > 1.0f) x = 1.0f;
    return (powf(EXP_BASE, x) - 1) / (EXP_BASE - 1);
}

static int GetEnvelopeValue(const Envelope *envelope, int time, int stopTime, bool active)
{
    time = GetPlayingTime(envelope, time, stopTime, active);

    if (time >= ENVELOPE_LENGTH) {
        return 0;
    }

    return envelope->value[time];
}

static float GetNoteFrequency(Note note)
{
    return 440.0f * powf(2.0f, ((int)note - 49) / 12.0f);
}

static AudioFrame SoundQueuePop(void)
{
    SDL_LockMutex(soundQueueMutex);
    if (ShouldQuit() || soundQueueSize == 0) {
        SDL_UnlockMutex(soundQueueMutex);
        return SILENCE;
    }

    AudioFrame res = soundQueue[--soundQueueSize];

    SDL_UnlockMutex(soundQueueMutex);

    return res;
}

// Returns value in range [0, 1), to be passed
// to the wave generation function.
static float GetArgument(int channel)
{
    float pitch = (float)curEnvelope[channel][ENVELOPE_PITCH];
    pitch += 16.0f * (float)curEnvelope[channel][ENVELOPE_ARPEGGIO];

    float fr = baseFrequency[channel] * powf(2.0f, pitch / 12.0f / 16.0f);

    float t = fr * samplesSinceStart[channel] * 1.0f / SAMPLE_RATE;
    t += phaseShift[channel];
    t = fmodf(t, 1.0f);

    return t;
}

static void AudioCallback(void *userData, uint8_t *byteStream, int byteLen)
{
    (void)userData;
    float *stream = (float*)byteStream;
    int len = byteLen / sizeof(float);

    for (int i = 0; i < len; ++i) {
        if (curSampleIdx == SAMPLES_PER_FRAME) {
            curSampleIdx = 0;
            audioFrame = SoundQueuePop();
            for (int channel = 0; channel < SOUND_CHANNELS; ++channel) {
                ChannelState ch = audioFrame.channels[channel];
                if (ch.note == STOP_NOTE) {
                    continue;
                }

                int duration = audioFrame.time - ch.playingSince;
                int stopDuration = audioFrame.time - ch.stoppedSince;
                bool newNote = duration == 0 && ch.isPlaying;
                float t1 = 0.0f, t2 = 0.0f;

                if (newNote) {
                    samplesSinceStart[channel] = 0;
                    phaseShift[channel] = 0;
                } else {
                    t1 = GetArgument(channel);
                }

                baseFrequency[channel] = GetNoteFrequency(ch.note);

                for (int envelope = 0; envelope < NUMBER_OF_ENVELOPES; ++envelope) {
                    curEnvelope[channel][envelope] = GetEnvelopeValue(
                            &ch.instrument.envelopes[envelope],
                            duration / ch.instrument.speed,
                            stopDuration / ch.instrument.speed, ch.isPlaying
                    );

                    nextEnvelope[channel][envelope] = GetEnvelopeValue(
                            &ch.instrument.envelopes[envelope],
                            (duration + 1) / ch.instrument.speed,
                            (stopDuration + 1) / ch.instrument.speed, ch.isPlaying
                    );
                }

                if (!newNote) {
                    t2 = GetArgument(channel);
                    phaseShift[channel] += t1 - t2;
                }
            }
        }

        float totalValue = 0.0f;

        for (int channel = 0; channel < SOUND_CHANNELS; ++channel) {
            ChannelState ch = audioFrame.channels[channel];

            if (ch.note == STOP_NOTE) {
                continue;
            }

            float t = GetArgument(channel);
            float value = 0.0f;

            switch (ch.instrument.wave) {
            case NOISE:
                value = (float)rand() / (float)RAND_MAX;
                break;

            case SQUARE_WAVE: {
                int e = curEnvelope[channel][ENVELOPE_DUTY_CYCLE];
                float duty = .5f * (ENVELOPE_DUTY_CYCLE_MAX - e + 1) / (ENVELOPE_DUTY_CYCLE_MAX + 1);
                value = (t < duty) ? 1 : -1;
            }
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

            value *= Volume((float)ch.instrument.volume / (float)ENVELOPE_VOLUME_MAX);
            value *= ch.volume;

            // interpolate volume envelope
            float v1 = (float)curEnvelope[channel][ENVELOPE_VOLUME] / ENVELOPE_VOLUME_MAX;
            float v2 = (float)nextEnvelope[channel][ENVELOPE_VOLUME] / ENVELOPE_VOLUME_MAX;
            float x = (float)curSampleIdx / SAMPLES_PER_FRAME;
            value *= v2 * x + v1 * (1.0f - x);

            totalValue += value;

            ++samplesSinceStart[channel];
        }


        if (totalValue > 1.0f) {
            totalValue = 1.0f;
        }

        if (totalValue < -1.0f) {
            totalValue = -1.0f;
        }

        totalValue *= audioFrame.volume;

        *stream++ = totalValue;
        ++curSampleIdx;
    }
}

static void PopulateQueue(void)
{
    AudioFrame frame;
    frame.volume = masterVolume;
    for (int ch = 0; ch < SOUND_CHANNELS; ++ch) {
        if (channels[ch].instrument.speed == 0) {
            channels[ch].instrument.speed = 1;
        }

        int duration = (curFrame - channels[ch].stoppedSince) / channels[ch].instrument.speed;
        if (!channels[ch].isPlaying && duration >= ENVELOPE_LENGTH) {
            channels[ch].note = STOP_NOTE;
        }

        frame.channels[ch] = channels[ch];
        frame.time = curFrame;
    }

    AudioFrame *queue = soundQueueStorage[soundQueue == soundQueueStorage[0]];

    for (int idx = 0; idx < PRELOAD_SOUNDS; ++idx) {
        queue[PRELOAD_SOUNDS - 1 - idx] = frame;
        ++frame.time;
    }

    // Publish the queue
    SDL_LockMutex(soundQueueMutex);
    soundQueue = queue;
    soundQueueSize = PRELOAD_SOUNDS;
    SDL_UnlockMutex(soundQueueMutex);
}

static void AudioFrameCallback(void)
{
    ++curFrame;
    PopulateQueue();
}

#ifdef SIG8_COMPILE_EDITORS
static void OnEditor(void)
{
    for (int i = 0; i < SOUND_CHANNELS; ++i) {
        StopNote(i);
    }
}
#endif

void sig8_InitAudio(void)
{
    SetMasterVolume(0.5f);
    sig8_RegisterCallback(FRAME_EVENT, AudioFrameCallback);
#ifdef SIG8_COMPILE_EDITORS
    sig8_RegisterCallback(EDITOR_ENTER_EVENT, OnEditor);
    sig8_RegisterCallback(EDITOR_LEAVE_EVENT, OnEditor);
#endif

    soundQueue = soundQueueStorage[0];
    for (int i = 0; i < SOUND_CHANNELS; ++i) {
        SILENCE.channels[i].note = STOP_NOTE;
        channels[i].volume = 1.0f;
    }

    soundQueueMutex = SDL_CreateMutex();
    SDL_AudioSpec want, have;
    SDL_zero(want);

    want.freq = SAMPLE_RATE;
    want.channels = 1;
    want.samples = SAMPLE_SIZE;
    want.format = AUDIO_F32;
    want.callback = AudioCallback;

    audioDevice = SDL_OpenAudioDevice(NULL, 0, &want, &have, 0);
    if (audioDevice == 0) {
        puts("Failed to obtain audio device");
        Finalize();
        exit(EXIT_FAILURE);
    }

    SDL_PauseAudioDevice(audioDevice, 0);
}

Instrument NewInstrument(void)
{
    Instrument res;
    memset(&res, 0, sizeof res);

    res.speed = 1;
    res.volume = ENVELOPE_VOLUME_MAX;
    res.wave = SQUARE_WAVE;
    res.envelopes[ENVELOPE_VOLUME].value[0] = ENVELOPE_VOLUME_MAX;
    res.envelopes[ENVELOPE_VOLUME].loopEnd = 1;

    return res;
}

void UseInstrument(Instrument instrument, int channel)
{
    channels[channel].instrument = instrument;
    PopulateQueue();
}

void SetMasterVolume(float volume)
{
    masterVolume = Volume(volume);
}

void SetChannelVolume(int channel, float volume)
{
    channels[channel].volume = volume;
}

void PlayNote(Note note, int channel)
{
    if (note == STOP_NOTE) {
        StopNote(channel);
        return;
    }

    channels[channel].playingSince = curFrame;
    channels[channel].isPlaying = true;
    channels[channel].note = note;
}

void StopNote(int channel)
{
    channels[channel].stoppedSince = curFrame;
    channels[channel].isPlaying = false;
}
