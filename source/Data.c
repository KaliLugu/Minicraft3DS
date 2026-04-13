#include "Data.h"

void initData() {
    uint16_t modCount = 0;
    // TODO: scanner sdmc:/mods/ ici
    itemsTableBuild(modCount);
    itemsDataInit();
    tilesDataInit();
    effectsDataInit();
    entityDataInit();
}
