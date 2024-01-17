#include "../Entity.h"

#include "../Data.h"
#include "../Render.h"

Entity newEntityNPC(int type, int x, int y, uByte level) {
    Entity e = {0}; // NOTE: always set to 0 to prevent uninitialized garbage data from causing issues (desyncs)
    e.type = ENTITY_NPC;
    e.level = level;
    e.x = x;
    e.y = y;
    e.hurtTime = 0;
    e.xKnockback = 0;
    e.yKnockback = 0;
    e.npc.type = type;
    e.xr = 4;
    e.yr = 3;
    e.canPass = false;

    return e;
}

void renderEntityNPC(Entity *e, sInt x, sInt y) {
    renderEntityShadow(x, y);
    renderTile16(x - 8, y - 8, e->npc.type, 4, 0);
}
