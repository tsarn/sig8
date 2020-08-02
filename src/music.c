#include "sig8_internal.h"
#include "stb_image.h"

static MusicLib currentMusicLib;

MusicLib LoadMusicLib(const char *path)
{
    MusicLib result = (MusicLib)sig8_AllocateResource(RESOURCE_MUSICLIB, path, MUSICLIB_BYTE_SIZE);

    for (int i = 0; i < MUSICLIB_SIZE; ++i) {
        result[i].tempo = 12;
    }

    int compressedSize;
    char *compressed = (char *)ReadFileContents(path, &compressedSize);

    if (!compressed) {
        printf("WARNING: Failed to load music lib '%s'\n", path);
        return result;
    }

    stbi_zlib_decode_buffer((char *)result, MUSICLIB_BYTE_SIZE, compressed, compressedSize);
    free(compressed);

    return result;
}

MusicLib GetCurrentMusicLib(void)
{
    return currentMusicLib;
}

void FreeMusicLib(MusicLib musicLib)
{
    sig8_FreeResource(musicLib);
}

void UseMusicLib(MusicLib musicLib)
{
    currentMusicLib = musicLib;
}

void PlayTrack(int track)
{
}

void StopTrack(void)
{
}
