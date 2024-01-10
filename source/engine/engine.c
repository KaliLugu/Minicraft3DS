#include "engine.h"
#include "audio.h"
#include "input.h"
#include "network.h"
#include "platform.h"

void initEngine(PacketHandler ph) {
    initPlatform();
    initGraphics();
    initAudio();
    initNetwork(ph);
}

void exitEngine() {
    exitNetwork();
    exitAudio();
    exitGraphics();
    exitPlatform();
}
