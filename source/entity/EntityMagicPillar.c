#include "../Entity.h"

#include "../Data.h"

Entity newEntityMagicPillar(int x, int y, uByte level) {
    Entity e = {0}; // NOTE: always set to 0 to prevent uninitialized garbage data from causing issues (desyncs)
    e.type = ENTITY_MAGIC_PILLAR;
    e.level = level;
    e.x = x;
    e.y = y;
    e.xr = 3;
    e.yr = 3;
    e.canPass = false;

    e.tickFunction = NULL;

    return e;
}
