#include "sig8_internal.h"

static SoundLib currentSoundLib;

SoundLib LoadSoundLib(const char *path)
{
    SoundLib result = sig8_AllocateResource(RESOURCE_SOUNDLIB, path, SOUNDLIB_BYTE_SIZE);

    int size;
    uint8_t *data = ReadFileContents(path, &size);

    if (!data || size != SOUNDLIB_BYTE_SIZE) {
        printf("WARNING: Failed to load sound lib '%s'\n", path);
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
    SetInstrument(channel, currentSoundLib[sound].instrument);
    PlayNote(currentSoundLib[sound].note, channel);
}
