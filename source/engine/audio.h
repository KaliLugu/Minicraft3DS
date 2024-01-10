#pragma once
#include "dtypes.h"

typedef struct _sound _sound;
typedef _sound *Sound;

typedef struct _music _music;
typedef _music *Music;

extern void initAudio();
extern void exitAudio();

extern Sound loadSound(char *name);
extern void playSound(Sound sound);
extern void unloadSound(Sound sound);

extern Music loadMusic(char *name);
extern void playMusic(Music music);
extern void stopMusic();
extern void unloadMusic(Music music);
