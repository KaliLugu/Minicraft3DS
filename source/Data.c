#include "Data.h"
    // TODO: scanner sdmc:/3ds/Minicraft3ds/mods/ ici

void initData() {
    uint16_t modCount = 0;
    itemsTableBuild(modCount);
    effectsTableBuild(modCount);
    tilesDataInit();
    entityDataInit();
}
