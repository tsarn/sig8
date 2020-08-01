#include "sig8_internal.h"

static SoundLib currentSoundLib;

SoundLib LoadSoundLib(const char *path)
{
    SoundLib result = sig8_AllocateResource(RESOURCE_SOUNDLIB, path, SOUNDLIB_BYTE_SIZE);

    int size;
    uint8_t *data = ReadFileContents(path, &size);

    if (!data || size != SOUNDLIB_BYTE_SIZE) {
        printf("WARNING: Failed to load sound lib '%s'\n", path);
        for (int i = 0; i < SOUNDLIB_SIZE; ++i) {
            result[i].note = C4;
            result[i].instrument = NewInstrument();
        }
        return result;
    }

    memcpy(result, data, SOUNDLIB_BYTE_SIZE);
    free(data);
    return result;
}

SoundLib GetCurrentSoundLib(void)
{
    return currentSoundLib;
}

void FreeSoundLib(SoundLib soundLib)
{
    sig8_FreeResource(soundLib);
}

void UseSoundLib(SoundLib soundLib)
{
    currentSoundLib = soundLib;
}

void PlaySound(int sound, int channel)
{
    UseInstrument(currentSoundLib[sound].instrument, channel);
    PlayNote(currentSoundLib[sound].note, channel);
}
