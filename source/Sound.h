#pragma once
#include "engine/engine.h"

extern void playSoundPositioned(Sound snd, uByte level, int x, int y);
extern void setListenerPosition(uByte level, int x, int y);

extern void updateMusic(uByte level, int time);

extern void loadSounds();
extern void freeSounds();

extern Sound snd_playerHurt;
extern Sound snd_playerDeath;
extern Sound snd_monsterHurt;
extern Sound snd_test;
extern Sound snd_pickup;
extern Sound snd_bossdeath;
extern Sound snd_craft;

extern Music music_menu;
extern Music music_floor0;
extern Music music_floor1;
extern Music music_floor1_night;
extern Music music_floor23;
extern Music music_floor4;
