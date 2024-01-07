#pragma once
#include <3ds.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// Note: The lower channels (<8 ?) seem to not work?
#define SOUND_CHANNEL_MIN 15
#define SOUND_CHANNEL_MAX 23
#define MUSIC_CHANNEL 10

#define MUSIC_STREAM_BUFFER_SIZE (65536 * 8)

typedef struct
{
    u32 size;
    u8 *buffer;
} Sound;

typedef struct
{
    u32 size;
    u32 pos;
    char *filename;
} Music;

extern void loadSound(Sound *snd, char *filename);
extern void playSound(Sound snd);
extern void playSoundPositioned(Sound snd, s8 level, int x, int y);
extern void setListenerPosition(s8 level, int x, int y);

extern void loadMusic(Music *music, char *filename);
extern void playMusic(Music *music);
extern void updateMusic(int lvl, int time);
extern void stopMusic();

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