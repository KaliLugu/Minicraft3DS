#include "../engine.h"
#ifdef __3DS__
#include "audio.h"
#include <stdio.h>

#define SOUND_CHANNEL_MIN 15
#define SOUND_CHANNEL_MAX 23
#define MUSIC_CHANNEL 10

#define MUSIC_STREAM_BUFFER_SIZE (65536 * 8)

// sound channel data
ndspWaveBuf *channelBuf;

// music stream data
s16 *musicBuffer1;
s16 *musicBuffer2;
s16 *musicBufferCurrent;
u32 musicBufferCurrentSamples;
ndspWaveBuf *streamBuffer1;
ndspWaveBuf *streamBuffer2;
ndspWaveBuf *streamBufferCurrent;
Music currentMusic;

bool runMusicThread;
Thread musicThread;

void musicStreamThreadMain(void *arg) {
    while (runMusicThread) {
        if (!ndspChnIsPlaying(MUSIC_CHANNEL) && currentMusic != NULL) {
            // play current buffer
            streamBufferCurrent->data_pcm16 = musicBufferCurrent;
            streamBufferCurrent->nsamples = musicBufferCurrentSamples;
            ndspChnWaveBufAdd(MUSIC_CHANNEL, streamBufferCurrent);

            // switch buffer
            if (musicBufferCurrent == musicBuffer1) {
                musicBufferCurrent = musicBuffer2;
                streamBufferCurrent = streamBuffer2;
            } else {
                musicBufferCurrent = musicBuffer1;
                streamBufferCurrent = streamBuffer1;
            }

            // load next buffer
            u32 bufferSize = MUSIC_STREAM_BUFFER_SIZE;
            if (currentMusic->size - currentMusic->pos < bufferSize) {
                bufferSize = currentMusic->size - currentMusic->pos;
            }
            FILE *file = fopen(currentMusic->name, "rb");
            if (file != NULL) {
                fseek(file, currentMusic->pos, SEEK_SET);
                fread(musicBufferCurrent, 1, bufferSize, file);
                musicBufferCurrentSamples = bufferSize / 2;

                currentMusic->pos = currentMusic->pos + bufferSize;
                if (currentMusic->pos >= currentMusic->size) {
                    currentMusic->pos = 0;
                }
            } else {
                currentMusic = NULL;
            }
            fclose(file);
        }
    }
}

void initAudio() {
    ndspInit();
    ndspSetOutputMode(NDSP_OUTPUT_STEREO);

    channelBuf = linearAlloc(sizeof(ndspWaveBuf) * (SOUND_CHANNEL_MAX + 1));
    for (int c = SOUND_CHANNEL_MIN; c <= SOUND_CHANNEL_MAX; c++) {
        ndspChnSetFormat(c, NDSP_FORMAT_MONO_PCM16);
        ndspChnSetRate(c, 44100);
    }
    ndspChnSetFormat(MUSIC_CHANNEL, NDSP_FORMAT_MONO_PCM16);
    ndspChnSetRate(MUSIC_CHANNEL, 44100);

    musicBuffer1 = linearAlloc(MUSIC_STREAM_BUFFER_SIZE);
    musicBuffer2 = linearAlloc(MUSIC_STREAM_BUFFER_SIZE);
    musicBufferCurrent = musicBuffer1;
    streamBuffer1 = linearAlloc(sizeof(ndspWaveBuf));
    streamBuffer2 = linearAlloc(sizeof(ndspWaveBuf));
    streamBufferCurrent = streamBuffer1;
    currentMusic = NULL;

    // start streaming thread (with lower priority (->higher value) than main and importantly network thread)
    runMusicThread = true;
    s32 prio = 0;
    svcGetThreadPriority(&prio, CUR_THREAD_HANDLE);
    musicThread = threadCreate(musicStreamThreadMain, NULL, 1024, prio + 1, -2, false);
}

void exitAudio() {
    // stop streaming thread
    if (runMusicThread) {
        runMusicThread = false;
        threadJoin(musicThread, U64_MAX);
        threadFree(musicThread);
    }

    // stop all sounds
    ndspChnWaveBufClear(MUSIC_CHANNEL);
    for (int c = SOUND_CHANNEL_MIN; c <= SOUND_CHANNEL_MAX; c++) {
        ndspChnWaveBufClear(c);
    }

    // free buffers
    linearFree(musicBuffer1);
    linearFree(musicBuffer2);
    linearFree(streamBuffer1);
    linearFree(streamBuffer2);

    linearFree(channelBuf);

    ndspExit();
}

Sound loadSound(char *name) {
    Sound sound = linearAlloc(sizeof(_3ds_sound));

    FILE *file = fopen(name, "rb");
    if (file != NULL) {
        fseek(file, 0, SEEK_END);
        sound->count = ftell(file) / 2;
        fseek(file, 0, SEEK_SET);
        sound->samples = linearAlloc(sound->count * 2);
        fread(sound->samples, 2, sound->count, file);
    }
    fclose(file);

    return sound;
}

void playSound(Sound sound) {
    if (sound == NULL)
        return;
    if (sound->samples == NULL)
        return;

    for (int c = SOUND_CHANNEL_MIN; c <= SOUND_CHANNEL_MAX; c++) {
        if (!ndspChnIsPlaying(c)) {
            channelBuf[c].data_pcm16 = sound->samples;
            channelBuf[c].nsamples = sound->count;
            ndspChnWaveBufAdd(c, channelBuf + c);
            return;
        }
    }
}

void unloadSound(Sound sound) {
    if (sound == NULL)
        return;
    if (sound->samples != NULL) {
        linearFree(sound->samples);
        sound->samples = NULL;
    }
    linearFree(sound);
}

Music loadMusic(char *name) {
    Music music = linearAlloc(sizeof(_3ds_music));

    FILE *file = fopen(name, "rb");
    if (file != NULL) {
        fseek(file, 0, SEEK_END);
        music->size = ftell(file);
        music->pos = 0;
        music->name = name;
    }
    fclose(file);

    return music;
}

void playMusic(Music music) {
    if (currentMusic == music)
        return;

    stopMusic();

    music->pos = 0;

    // load first buffer
    u32 bufferSize = MUSIC_STREAM_BUFFER_SIZE;
    if (music->size - music->pos < bufferSize) {
        bufferSize = music->size - music->pos;
    }
    FILE *file = fopen(music->name, "rb");
    if (file != NULL) {
        fseek(file, music->pos, SEEK_SET);
        fread(musicBufferCurrent, 1, bufferSize, file);
        musicBufferCurrentSamples = bufferSize / 2;

        music->pos = music->pos + bufferSize;
        if (music->pos >= music->size) {
            music->pos = 0;
        }
        currentMusic = music;
    } else {
        currentMusic = NULL;
    }
    fclose(file);
}

void stopMusic() {
    ndspChnWaveBufClear(MUSIC_CHANNEL);
    ndspChnReset(MUSIC_CHANNEL);
    ndspChnSetFormat(MUSIC_CHANNEL, NDSP_FORMAT_MONO_PCM16);
    ndspChnSetRate(MUSIC_CHANNEL, 44100);
    currentMusic = NULL;
}

void unloadMusic(Music music) {
    if (music == NULL)
        return;
    linearFree(music);
}

#endif
