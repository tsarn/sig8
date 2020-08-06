#include <sig8.h>
#include "sig8_internal.h"
#include "stb_image.h"

static MusicLib currentMusicLib;
static SoundLib currentSoundLib;

static bool isMusicPlaying;
static int currentTrack;
static int currentFragment;
static int currentRow;
static int duration;

static void MusicCallback()
{
    if (!isMusicPlaying) {
        return;
    }

    if (duration <= 0) {
        ++currentRow;
        if (currentRow >= PATTERN_LENGTH) {
            currentRow = 0;
            isMusicPlaying = false;
            return;
        }

        duration = 6 * FRAME_RATE / currentMusicLib[currentTrack].tempo;

        for (int i = 0; i < MUSIC_CHANNELS; ++i) {
            int pattern = currentMusicLib[currentTrack].fragments[currentFragment][i] - 1;
            if (pattern == -1) {
                continue;
            }
            int instrument = currentMusicLib[currentTrack].patterns[pattern].notes[currentRow].instrument;
            int note = currentMusicLib[currentTrack].patterns[pattern].notes[currentRow].note;
            if (note == STOP_NOTE) {
                StopNote(i);
            } else {
                UseInstrument(currentSoundLib[instrument].instrument, i);
                PlayNote(note, i);
            }
        }
    }

    --duration;
}

void sig8_GetMusicState(bool *playing, int *track, int *fragment, int *row)
{
    if (playing) *playing = isMusicPlaying;
    if (track) *track = currentTrack;
    if (fragment) *fragment = currentFragment;
    if (row) *row = currentRow;
}

void sig8_InitMusic(void)
{
    sig8_RegisterCallback(FRAME_EVENT, MusicCallback);
    sig8_RegisterCallback(EDITOR_ENTER_EVENT, StopTrack);
    sig8_RegisterCallback(EDITOR_LEAVE_EVENT, StopTrack);
}

MusicLib LoadMusicLib(const char *path)
{
    MusicLib result = (MusicLib)sig8_AllocateResource(RESOURCE_MUSICLIB, path, MUSICLIB_BYTE_SIZE);

    for (int i = 0; i < MUSICLIB_SIZE; ++i) {
        result[i].tempo = 20;
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
    isMusicPlaying = true;
    currentSoundLib = GetCurrentSoundLib();
    currentTrack = track;
    currentFragment = 0;
    currentRow = -1;
    duration = 0;
}

void StopTrack(void)
{
    isMusicPlaying = false;
    for (int i = 0; i < MUSIC_CHANNELS; ++i) {
        StopNote(i);
    }
}
