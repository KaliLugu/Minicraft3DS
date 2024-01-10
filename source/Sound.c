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

int soundListenerLevel;
int soundListenerX;
int soundListenerY;

void playSoundPositioned(Sound snd, uByte level, int x, int y) {
    if (level != soundListenerLevel)
        return;
    int xd = soundListenerX - x;
    int yd = soundListenerY - y;
    if (xd * xd + yd * yd > 80 * 80)
        return;

    playSound(snd);
}

void setListenerPosition(uByte level, int x, int y) {
    soundListenerLevel = level;
    soundListenerX = x;
    soundListenerY = y;
}

void updateMusic(uByte level, int time) {
    switch (level) {
    case 0:
        playMusic(music_floor0);
        break;
    case 1:
        if (time > 6000 && time < 19000)
            playMusic(music_floor1);
        else
            playMusic(music_floor1_night);
        break;
    case 2:
    case 3:
        playMusic(music_floor23);
        break;
    case 4:
    case 5: // TODO - dungeon needs own music
        playMusic(music_floor4);
        break;
    }
}

void loadSounds() {
    // create and load buffers
    snd_playerHurt = loadSound("romfs:/playerhurt.raw");
    snd_playerDeath = loadSound("romfs:/playerdeath.raw");
    snd_monsterHurt = loadSound("romfs:/monsterhurt.raw");
    snd_test = loadSound("romfs:/test.raw");
    snd_pickup = loadSound("romfs:/pickup.raw");
    snd_bossdeath = loadSound("romfs:/bossdeath.raw");
    snd_craft = loadSound("romfs:/craft.raw");

    music_menu = loadMusic("romfs:/music/menu.raw");
    music_floor0 = loadMusic("romfs:/music/floor0.raw");
    music_floor1 = loadMusic("romfs:/music/floor1.raw");
    music_floor1_night = loadMusic("romfs:/music/floor1_night.raw");
    music_floor23 = loadMusic("romfs:/music/floor2_3.raw");
    music_floor4 = loadMusic("romfs:/music/floor4.raw");
}

void freeSounds() {
    unloadSound(snd_playerHurt);
    unloadSound(snd_playerDeath);
    unloadSound(snd_monsterHurt);
    unloadSound(snd_test);
    unloadSound(snd_pickup);
    unloadSound(snd_bossdeath);
    unloadSound(snd_craft);

    unloadMusic(music_menu);
    unloadMusic(music_floor0);
    unloadMusic(music_floor1);
    unloadMusic(music_floor1_night);
    unloadMusic(music_floor23);
    unloadMusic(music_floor4);
}
