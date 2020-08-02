#include "sig8_internal.h"
#include "stb_image.h"

static SoundLib currentSoundLib;

SoundLib LoadSoundLib(const char *path)
{
    SoundLib result = (SoundLib)sig8_AllocateResource(RESOURCE_SOUNDLIB, path, SOUNDLIB_BYTE_SIZE);

    for (int i = 0; i < SOUNDLIB_SIZE; ++i) {
        result[i].note = C4;
        result[i].instrument = NewInstrument();
    }

    int compressedSize;
    char *compressed = (char *)ReadFileContents(path, &compressedSize);

    if (!compressed) {
        printf("WARNING: Failed to load sound lib '%s'\n", path);
        return result;
    }

    stbi_zlib_decode_buffer((char *)result, SOUNDLIB_BYTE_SIZE, compressed, compressedSize);
    free(compressed);

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
