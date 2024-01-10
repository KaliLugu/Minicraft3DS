#ifdef __SWITCH__
#include "../engine.h"

struct _sound {
};

struct _music {
};

void initAudio() {
}

void exitAudio() {
}

Sound loadSound(char *name) {
    return NULL;
}

void playSound(Sound sound) {
}

void unloadSound(Sound sound) {
}

Music loadMusic(char *name) {
    return NULL;
}

void playMusic(Music music) {
}

void stopMusic() {
}

void unloadMusic(Music music) {
}

#endif
