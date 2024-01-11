#ifdef __SWITCH__
#include "../engine.h"

#include <raylib.h>
#include <stdio.h>
#include <switch.h>

struct _sound {
    Sound sound;
};

struct _music {
    Music music;
};

static MMusic currentMusic = NULL;

static volatile bool runMusicThread;
static Thread musicThread;

static void musicStreamThreadMain(void *arg) {
    while (runMusicThread) {
        if (currentMusic != NULL) {
            UpdateMusicStream(currentMusic->music);
        }
        svcSleepThread(1000 * 1000);
    }
}

void initAudio() {
    InitAudioDevice();

    // start streaming thread (with lower priority (->higher value) than main and importantly network thread)
    runMusicThread = true;
    threadCreate(&musicThread, musicStreamThreadMain, NULL, NULL, 0x10000, 0x2C, -2);
    threadStart(&musicThread);
}

void exitAudio() {
    // stop streaming thread
    if (runMusicThread) {
        runMusicThread = false;
        threadWaitForExit(&musicThread);
        threadClose(&musicThread);
    }

    CloseAudioDevice();
}

MSound loadSound(char *name) {
    FILE *file = fopen(name, "rb");
    if (file != NULL) {
        fseek(file, 0, SEEK_END);
        long frameCount = ftell(file) / 2;
        fseek(file, 0, SEEK_SET);
        void *data = malloc(frameCount * 2);
        fread(data, 2, frameCount, file);
        fclose(file);

        MSound sound = malloc(sizeof(_sound));

        Wave wave = {frameCount, 44100, 16, 1, data};
        sound->sound = LoadSoundFromWave(wave);
        free(data);

        return sound;
    }

    return NULL;
}

void playSound(MSound sound) {
    if (sound == NULL)
        return;

    PlaySound(sound->sound);
}

void unloadSound(MSound sound) {
    if (sound == NULL)
        return;

    UnloadSound(sound->sound);
    free(sound);
}

MMusic loadMusic(char *name) {
    MMusic music = malloc(sizeof(_music));
    music->music = LoadMusicStream(name);
    return music;
}

void playMusic(MMusic music) {
    if (currentMusic == music)
        return;

    stopMusic();
    PlayMusicStream(music->music);
    currentMusic = music;
}

void stopMusic() {
    if (currentMusic == NULL)
        return;

    StopMusicStream(currentMusic->music);
    currentMusic = NULL;
}

void unloadMusic(MMusic music) {
    if (music == NULL)
        return;
    UnloadMusicStream(music->music);
    free(music);
}

#endif
