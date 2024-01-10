#include "engine.h"
#include "audio.h"

void initEngine(DrawCallback cb) {
    initGraphics(cb);
    initAudio();
}

void exitEngine() {
    exitAudio();
    exitGraphics();
}
