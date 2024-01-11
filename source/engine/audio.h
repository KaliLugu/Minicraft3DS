#pragma once
#include "dtypes.h"

typedef struct _sound _sound;
typedef _sound *MSound;

typedef struct _music _music;
typedef _music *MMusic;

extern void initAudio();
extern void exitAudio();

extern MSound loadSound(char *name);
extern void playSound(MSound sound);
extern void unloadSound(MSound sound);

extern MMusic loadMusic(char *name);
extern void playMusic(MMusic music);
extern void stopMusic();
extern void unloadMusic(MMusic music);
