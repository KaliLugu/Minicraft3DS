#include "../Entity.h"

#include "../Data.h"
#include "../Render.h"
// TODO: remove this roundabout include
#include "../network/Synchronizer.h"

Entity newParticleText(char *str, MColor color, int x, int y, uByte level) {
    Entity e = {0}; // NOTE: always set to 0 to prevent uninitialized garbage data from causing issues (desyncs)
    e.type = ENTITY_TEXTPARTICLE;
    e.level = level;
    e.textParticle.color = color;
    e.textParticle.age = 0;
    e.textParticle.text = (char *)calloc(strlen(str), sizeof(char));
    strncpy(e.textParticle.text, str, strlen(str));
    e.x = x;
    e.y = y;
    e.canPass = true;
    e.textParticle.xx = x;
    e.textParticle.yy = y;
    e.textParticle.zz = 2;
    e.textParticle.xa = syncGaussRand() * 0.3;
    e.textParticle.ya = syncGaussRand() * 0.2;
    e.textParticle.za = ((float)syncRand() / SYNC_RAND_MAX) * 0.7 + 2;

    return e;
}

void tickParticleText(Entity *e, PlayerData *nearestPlayer) {
    ++e->textParticle.age;
    if (e->textParticle.age == 60) {
        removeEntityFromList(e, e->level, &eManager);
        return;
    }
    e->textParticle.xx += e->textParticle.xa;
    e->textParticle.yy += e->textParticle.ya;
    e->textParticle.zz += e->textParticle.za;
    if (e->textParticle.zz < 0) {
        e->textParticle.zz = 0;
        e->textParticle.za *= -0.5;
        e->textParticle.xa *= 0.6;
        e->textParticle.ya *= 0.6;
    }
    e->textParticle.za -= 0.15;
    e->x = (int)e->textParticle.xx;
    e->y = (int)e->textParticle.yy;
}

void renderParticleText(Entity *e, sInt x, sInt y) {
    renderTextColor(e->textParticle.text, x + 1, y - (int)e->textParticle.zz + 1, 0x000000FF);
    renderTextColor(e->textParticle.text, x, y - (int)e->textParticle.zz, e->textParticle.color);
}
