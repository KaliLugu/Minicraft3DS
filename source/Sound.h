#pragma once
#include "engine/engine.h"

extern void playSoundPositioned(MSound snd, uByte level, int x, int y);
extern void setListenerPosition(uByte level, int x, int y);

extern void updateMusic(uByte level, int time);

extern void loadSounds();
extern void freeSounds();

extern MSound snd_playerHurt;
extern MSound snd_playerDeath;
extern MSound snd_monsterHurt;
extern MSound snd_test;
extern MSound snd_pickup;
extern MSound snd_bossdeath;
extern MSound snd_craft;

extern MMusic music_menu;
extern MMusic music_floor0;
extern MMusic music_floor1;
extern MMusic music_floor1_night;
extern MMusic music_floor23;
extern MMusic music_floor4;
