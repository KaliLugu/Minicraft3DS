#include "Entity.h"

#include "Data.h"

EntityManager eManager;
Entity nullEntity = {0};

void addEntityToList(Entity e, EntityManager *em) {
    e.slotNum = em->lastSlot[e.level];
    em->entities[e.level][em->lastSlot[e.level]] = e;
    em->lastSlot[e.level]++;
}

void removeEntityFromList(Entity *e, uByte level, EntityManager *em) {
    int i;
    if (em->entities[level][e->slotNum].type == ENTITY_TEXTPARTICLE)
        free(em->entities[level][e->slotNum].textParticle.text);
    for (i = e->slotNum; i < em->lastSlot[level]; ++i) {
        em->entities[level][i] = em->entities[level][i + 1]; // Move the items down.
        em->entities[level][i].slotNum = i;
    }
    em->lastSlot[level]--;
    em->entities[level][em->lastSlot[level]] = nullEntity; // Make the last slot null.
}
