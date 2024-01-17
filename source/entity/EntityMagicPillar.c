#include "../Entity.h"

#include "../Data.h"
#include "../Render.h"

Entity newEntityMagicPillar(int x, int y, uByte level) {
    Entity e = {0}; // NOTE: always set to 0 to prevent uninitialized garbage data from causing issues (desyncs)
    e.type = ENTITY_MAGIC_PILLAR;
    e.level = level;
    e.x = x;
    e.y = y;
    e.xr = 3;
    e.yr = 3;
    e.canPass = false;

    return e;
}

void renderEntityMagicPillar(Entity *e, sInt x, sInt y) {
    renderEntityShadow(x, y);
    renderTile16(x - 8, y - 8, 1, 20, 0);
}
