#pragma once

#include "3ds/audio.h"
#include "switch/audio.h"

extern void initAudio();
extern void exitAudio();

extern Sound loadSound(char *name);
extern void playSound(Sound sound);
extern void unloadSound(Sound sound);

extern Music loadMusic(char *name);
extern void playMusic(Music music);
extern void stopMusic();
extern void unloadMusic(Music music);
