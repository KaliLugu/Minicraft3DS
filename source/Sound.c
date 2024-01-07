#include "Sound.h"

Sound snd_playerHurt;
Sound snd_playerDeath;
Sound snd_monsterHurt;
Sound snd_test;
Sound snd_pickup;
Sound snd_bossdeath;
Sound snd_craft;

Music music_menu;
Music music_floor0;
Music music_floor1;
Music music_floor1_night;
Music music_floor23;
Music music_floor4;

u8 soundListenerLevel;
int soundListenerX;
int soundListenerY;

ndspWaveBuf *channelBuf;

void loadSound(Sound *snd, char *filename) {
    FILE *file = fopen(filename, "rb");
    if (file != NULL) {
        fseek(file, 0, SEEK_END);
        snd->size = ftell(file);
        fseek(file, 0, SEEK_SET);
        snd->buffer = linearAlloc(snd->size);
        fread(snd->buffer, 1, snd->size, file);
    }
    fclose(file);
}

void playSound(Sound snd) {
    int c;

    for (c = SOUND_CHANNEL_MIN; c <= SOUND_CHANNEL_MAX; c++) {
        if (!ndspChnIsPlaying(c)) {
            ndspChnSetFormat(c, NDSP_FORMAT_MONO_PCM16);
            ndspChnSetRate(c, 44100);

            channelBuf[c].data_pcm16 = (s16 *)snd.buffer;
            channelBuf[c].nsamples = snd.size / 2;
            ndspChnWaveBufAdd(c, channelBuf + c);
            return;
        }
    }
}

void playSoundPositioned(Sound snd, s8 level, int x, int y) {
    if (level != soundListenerLevel)
        return;
    int xd = soundListenerX - x;
    int yd = soundListenerY - y;
    if (xd * xd + yd * yd > 80 * 80)
        return;

    playSound(snd);
}

void setListenerPosition(s8 level, int x, int y) {
    soundListenerLevel = level;
    soundListenerX = x;
    soundListenerY = y;
}

// TODO: Music should be streamed not loaded completely into memory
u8 *musicBuffer1;
u8 *musicBuffer2;
u8 *musicBufferCurrent;
u32 musicBufferCurrentSize;
ndspWaveBuf *streamBuffer1;
ndspWaveBuf *streamBuffer2;
ndspWaveBuf *streamBufferCurrent;

Music *currentMusic;
bool runMusicThread;
Thread musicThread;

void loadMusic(Music *music, char *filename) {
    FILE *file = fopen(filename, "rb");
    if (file != NULL) {
        fseek(file, 0, SEEK_END);
        music->size = ftell(file);
        music->pos = 0;
        music->filename = filename;
    }
    fclose(file);
}

void playMusic(Music *music) {
    if (currentMusic == music)
        return;

    stopMusic();

    music->pos = 0;

    // load first buffer
    u32 bufferSize = MUSIC_STREAM_BUFFER_SIZE;
    if (music->size - music->pos < bufferSize) {
        bufferSize = music->size - music->pos;
    }
    FILE *file = fopen(music->filename, "rb");
    if (file != NULL) {
        fseek(file, music->pos, SEEK_SET);
        fread(musicBufferCurrent, 1, bufferSize, file);
        musicBufferCurrentSize = bufferSize;

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
    currentMusic = NULL;
}

void updateMusic(int lvl, int time) {
    switch (lvl) {
    case 0:
        playMusic(&music_floor0);
        break;
    case 1:
        if (time > 6000 && time < 19000)
            playMusic(&music_floor1);
        else
            playMusic(&music_floor1_night);
        break;
    case 2:
    case 3:
        playMusic(&music_floor23);
        break;
    case 4:
    case 5: // TODO - dungeon needs own music
        playMusic(&music_floor4);
        break;
    }
}

void musicStreamThreadMain(void *arg) {
    ndspChnSetFormat(MUSIC_CHANNEL, NDSP_FORMAT_MONO_PCM16);
    ndspChnSetRate(MUSIC_CHANNEL, 44100);

    while (runMusicThread) {
        if (!ndspChnIsPlaying(MUSIC_CHANNEL) && currentMusic != NULL) {
            // play current buffer
            streamBufferCurrent->data_pcm16 = (s16 *)musicBufferCurrent;
            streamBufferCurrent->nsamples = musicBufferCurrentSize / 2;
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
            FILE *file = fopen(currentMusic->filename, "rb");
            if (file != NULL) {
                fseek(file, currentMusic->pos, SEEK_SET);
                fread(musicBufferCurrent, 1, bufferSize, file);
                musicBufferCurrentSize = bufferSize;

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

void loadSounds() {
    channelBuf = linearAlloc(sizeof(ndspWaveBuf) * SOUND_CHANNEL_MAX);

    // create and load buffers
    loadSound(&snd_playerHurt, "romfs:/playerhurt.raw");
    loadSound(&snd_playerDeath, "romfs:/playerdeath.raw");
    loadSound(&snd_monsterHurt, "romfs:/monsterhurt.raw");
    loadSound(&snd_test, "romfs:/test.raw");
    loadSound(&snd_pickup, "romfs:/pickup.raw");
    loadSound(&snd_bossdeath, "romfs:/bossdeath.raw");
    loadSound(&snd_craft, "romfs:/craft.raw");

    musicBuffer1 = linearAlloc(MUSIC_STREAM_BUFFER_SIZE);
    musicBuffer2 = linearAlloc(MUSIC_STREAM_BUFFER_SIZE);
    musicBufferCurrent = musicBuffer1;
    streamBuffer1 = linearAlloc(sizeof(ndspWaveBuf));
    streamBuffer2 = linearAlloc(sizeof(ndspWaveBuf));
    streamBufferCurrent = streamBuffer1;
    currentMusic = NULL;

    loadMusic(&music_menu, "romfs:/music/menu.raw");
    loadMusic(&music_floor0, "romfs:/music/floor0.raw");
    loadMusic(&music_floor1, "romfs:/music/floor1.raw");
    loadMusic(&music_floor1_night, "romfs:/music/floor1_night.raw");
    loadMusic(&music_floor23, "romfs:/music/floor2_3.raw");
    loadMusic(&music_floor4, "romfs:/music/floor4.raw");

    // start streaming thread (with lower priority (->higher value) than main and importantly network thread)
    runMusicThread = true;
    s32 prio = 0;
    svcGetThreadPriority(&prio, CUR_THREAD_HANDLE);
    musicThread = threadCreate(musicStreamThreadMain, NULL, 1024, prio + 1, -2, false);
}

void freeSounds() {
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
    linearFree(snd_playerHurt.buffer);
    linearFree(snd_playerDeath.buffer);
    linearFree(snd_monsterHurt.buffer);
    linearFree(snd_test.buffer);
    linearFree(snd_pickup.buffer);
    linearFree(snd_bossdeath.buffer);
    linearFree(snd_craft.buffer);

    linearFree(musicBuffer1);
    linearFree(musicBuffer2);
    linearFree(streamBuffer1);
    linearFree(streamBuffer2);

    linearFree(channelBuf);
}
