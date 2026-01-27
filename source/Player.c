#include "Player.h"
#include "data/items/ItemsData.h"

#include "Globals.h"
#include <limits.h>

PlayerData players[MAX_PLAYERS];
int playerCount;
int playerLocalIndex;

void initPlayers() {
    for (int i = 0; i < MAX_PLAYERS; i++) {
        initPlayer(players + i);
    }
}

void freePlayers() {
    for (int i = 0; i < MAX_PLAYERS; i++) {
        freePlayer(players + i);
    }
}

void playerInitMiniMapData(uByte *minimapData) {
    for (int i = 0; i < 128 * 128; i++) {
        minimapData[i] = 0;
    }
}

void playerInitEntity(PlayerData *pd) {
    pd->entity.type = ENTITY_PLAYER;
    pd->entity.level = 1;
    pd->entity.xr = 4;
    pd->entity.yr = 3;
    pd->entity.canSwim = true;
    pd->entity.p.ax = 0;
    pd->entity.p.ay = 0;
    pd->entity.p.health = 10;
    pd->entity.p.stamina = 10;
    pd->entity.p.walkDist = 0;
    pd->entity.p.attackTimer = 0;
    pd->entity.p.dir = 0;
    pd->entity.p.isDead = false;
    pd->entity.p.hasWon = false;

    pd->entity.p.data = pd;
}

void playerInitInventory(PlayerData *pd) {
    // reset inventory
    pd->inventory.lastSlot = 0;
    pd->activeItem = &noItem;

    addItemToInventory(newItem(itemGetLegacyId((ItemID){ITEM_CATEGORY_FURNITURE, 4}), 0), &(pd->inventory));
    addItemToInventory(newItem(itemGetLegacyId((ItemID){ITEM_CATEGORY_TOOL, 5}), 0), &(pd->inventory));

    if (TESTGODMODE) {
        addItemToInventory(newItem(itemGetLegacyId((ItemID){ITEM_CATEGORY_TOOL, 0}), 4), &(pd->inventory));
        addItemToInventory(newItem(itemGetLegacyId((ItemID){ITEM_CATEGORY_TOOL, 1}), 4), &(pd->inventory));
        addItemToInventory(newItem(itemGetLegacyId((ItemID){ITEM_CATEGORY_TOOL, 2}), 4), &(pd->inventory));
        addItemToInventory(newItem(itemGetLegacyId((ItemID){ITEM_CATEGORY_TOOL, 3}), 4), &(pd->inventory));
        addItemToInventory(newItem(itemGetLegacyId((ItemID){ITEM_CATEGORY_TOOL, 4}), 4), &(pd->inventory));

        addItemToInventory(newItem(itemGetLegacyId((ItemID){ITEM_CATEGORY_FURNITURE, 0}), 0), &(pd->inventory));
        addItemToInventory(newItem(itemGetLegacyId((ItemID){ITEM_CATEGORY_FURNITURE, 1}), 0), &(pd->inventory));
        addItemToInventory(newItem(itemGetLegacyId((ItemID){ITEM_CATEGORY_FURNITURE, 2}), 0), &(pd->inventory));
        addItemToInventory(newItem(itemGetLegacyId((ItemID){ITEM_CATEGORY_FURNITURE, 3}), 0), &(pd->inventory));
        addItemToInventory(newItem(itemGetLegacyId((ItemID){ITEM_CATEGORY_FURNITURE, 5}), 0), &(pd->inventory));

        addItemToInventory(newItem(itemGetLegacyId((ItemID){ITEM_CATEGORY_TOOL, 8}), 1), &(pd->inventory));

        int i;
        for (i = 7; i < 28; ++i)
            addItemToInventory(newItem(i, 50), &(pd->inventory));
        for (i = 51; i < 76; ++i)
            addItemToInventory(newItem(i, 50), &(pd->inventory));
        for (i = 1001; i < 1008; ++i)
            addItemToInventory(newItem(i, 50), &(pd->inventory));
    }
}

void playerInitEffects(PlayerData *pd) {
    int i;
    for (i = 0; i < EFFECTS_MAX; i++) {
        pd->effects[i].level = 0;
        pd->effects[i].time = 0;
    }
}

void playerInitSprite(PlayerData *pd) {
    pd->sprite.choosen = false;

    pd->sprite.legs = 0;
    pd->sprite.body = 0;
    pd->sprite.arms = 0;
    pd->sprite.head = 0;
    pd->sprite.eyes = 0;
    pd->sprite.accs = 0;
}

void playerInitMenus(PlayerData *pd) {
    pd->ingameMenu = MENU_NONE;
    pd->ingameMenuSelection = 0;
    pd->ingameMenuInvSel = 0;
    pd->ingameMenuInvSelOther = 0;
    pd->ingameMenuAreYouSure = false;
    pd->ingameMenuAreYouSureSave = false;
    pd->ingameMenuTimer = 0;

    resetNPCMenuData(&(pd->npcMenuData));

    pd->mapShouldRender = false;
    pd->mapScrollX = 0;
    pd->mapScrollY = 0;
    pd->mapZoomLevel = 2;
    sprintf(pd->mapText, "x%d", pd->mapZoomLevel);

    pd->touchLastX = -1;
    pd->touchLastY = -1;
    pd->touchIsDraggingMap = false;
    pd->touchIsChangingSize = false;
}

void initPlayer(PlayerData *pd) {
    pd->isSpawned = false;

    playerInitMiniMapData(pd->minimapData);
    playerInitEntity(pd);
    playerInitInventory(pd);
    playerInitEffects(pd);

    playerInitSprite(pd);

    initQuests(&(pd->questManager));
    resetQuests(&(pd->questManager));

    playerInitMenus(pd);

    pd->score = 0;
}

void freePlayer(PlayerData *pd) {
    freeQuests(&(pd->questManager));
}

PlayerData *getNearestPlayer(uByte level, int x, int y) {
    sInt nearest = -1;
    sInt nearestDist = INT_MAX;

    for (int i = 0; i < playerCount; i++) {
        if (players[i].entity.level != level)
            continue;

        sInt xdif = players[i].entity.x - x;
        sInt ydif = players[i].entity.y - y;

        sInt dist = xdif * xdif + ydif * ydif;
        if (dist < nearestDist) {
            nearest = i;
            nearestDist = dist;
        } else if (dist == nearestDist && players[i].id < players[nearest].id) {
            nearest = i;
        }
    }

    if (nearest == -1)
        return NULL;

    return players + nearest;
}

PlayerData *getLocalPlayer() {
    return players + playerLocalIndex;
}

// player update functions
bool _playerUseItemEat(PlayerData *pd, int energy, int health) {
    if (pd->entity.p.health < 10 && playerUseEnergy(pd, energy)) {
        playerHeal(pd, health);
        --(pd->activeItem->countLevel);
        return true;
    }
    return false;
}

bool _playerUseItemScroll(PlayerData *pd, int effect, uByte level, sInt time) {
    if (!playerEffectActive(pd, effect)) {
        playerEffectApply(pd, effect, level, time);
        --(pd->activeItem->countLevel);
        return true;
    }
    return false;
}

bool playerUseItem(PlayerData *pd) {
    int aitemID = 0;
    Item *aitem;
    Item *item;

    // shooting arrows
    if (pd->activeItem->id == itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 7})) {
        // Vérifier les flèches dans l'inventaire
        int arrowIDs[] = {
            itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 27}),  // Wood Arrow
            itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 28}),  // Stone Arrow
            itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 29}),  // Iron Arrow
            itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 30}),  // Gold Arrow
            itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 31})   // Gem Arrow
        };

        for (int i = 0; i < 5; i++) {
            item = getItemFromInventory(arrowIDs[i], &(pd->inventory));
            if (item != NULL) {
                aitemID = arrowIDs[i];
                aitem = item;
                break;  // Utiliser la première flèche trouvée
            }
        }

        if (aitemID != 0) {
            --aitem->countLevel;
            if (isItemEmpty(aitem)) {
                removeItemFromInventory(aitem->slotNum, &(pd->inventory));
            }

            switch (pd->entity.p.dir) {
            case 0:
                addEntityToList(newEntityArrow(&(pd->entity), aitemID, 0, 2, pd->entity.level), &eManager);
                break;
            case 1:
                addEntityToList(newEntityArrow(&(pd->entity), aitemID, 0, -2, pd->entity.level), &eManager);
                break;
            case 2:
                addEntityToList(newEntityArrow(&(pd->entity), aitemID, -2, 0, pd->entity.level), &eManager);
                break;
            case 3:
                addEntityToList(newEntityArrow(&(pd->entity), aitemID, 2, 0, pd->entity.level), &eManager);
                break;
            }
            return true;
        }
    }

    // Health items
    int healthItems[] = {
        itemGetLegacyId((ItemID){ITEM_CATEGORY_FOOD, 0}),
        itemGetLegacyId((ItemID){ITEM_CATEGORY_FOOD, 1}),
        itemGetLegacyId((ItemID){ITEM_CATEGORY_FOOD, 2}),
        itemGetLegacyId((ItemID){ITEM_CATEGORY_FOOD, 3}),
        itemGetLegacyId((ItemID){ITEM_CATEGORY_FOOD, 4}),
        itemGetLegacyId((ItemID){ITEM_CATEGORY_FOOD, 5}),
        itemGetLegacyId((ItemID){ITEM_CATEGORY_FOOD, 6}),
        itemGetLegacyId((ItemID){ITEM_CATEGORY_FOOD, 7})
    };

    for (int i = 0; i < 8; i++) {
        item = getItemFromInventory(healthItems[i], &(pd->inventory));
        if (item != NULL) {
            aitemID = healthItems[i];
            aitem = item;
            break; 
        }
    }

    // Utiliser l'item de santé actif
    if (pd->activeItem->id == itemGetLegacyId((ItemID){ITEM_CATEGORY_FOOD, 2})) { // Apple
        if (_playerUseItemEat(pd, 2, 1))
            return true;
    } else if (pd->activeItem->id == itemGetLegacyId((ItemID){ITEM_CATEGORY_FOOD, 3})) { // Golden Apple
        if (_playerUseItemEat(pd, 2, 7))
            return true;
    } else if (pd->activeItem->id == itemGetLegacyId((ItemID){ITEM_CATEGORY_FOOD, 0})) { // Flesh
        if (_playerUseItemEat(pd, 4 + (syncRand() % 4), 1))
            return true;
    } else if (pd->activeItem->id == itemGetLegacyId((ItemID){ITEM_CATEGORY_FOOD, 1})) { // Bread
        if (_playerUseItemEat(pd, 3, 2))
            return true;
    } else if (pd->activeItem->id == itemGetLegacyId((ItemID){ITEM_CATEGORY_FOOD, 4})) { // Raw Pork
        if (_playerUseItemEat(pd, 4 + (syncRand() % 4), 1))
            return true;
    } else if (pd->activeItem->id == itemGetLegacyId((ItemID){ITEM_CATEGORY_FOOD, 5})) { // Cooked Pork
        if (_playerUseItemEat(pd, 3, 3))
            return true;
    } else if (pd->activeItem->id == itemGetLegacyId((ItemID){ITEM_CATEGORY_FOOD, 6})) { // Raw Beef
        if (_playerUseItemEat(pd, 4 + (syncRand() % 4), 1))
            return true;
    } else if (pd->activeItem->id == itemGetLegacyId((ItemID){ITEM_CATEGORY_FOOD, 7})) { // Steak
        if (_playerUseItemEat(pd, 3, 4))
            return true;
    }

    // special item
    if (pd->activeItem == itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 34})) { // wizard summon
        if (pd->entity.level == 0) {
            --(pd->activeItem->countLevel);
            airWizardHealthDisplay = 2000;
            addEntityToList(newEntityAirWizard(630, 820, 0), &eManager);
        }
    }

    // scrolls
    if (pd->activeItem == itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 1001})) { // scrolls
        if (_playerUseItemScroll(pd, EFFECT_UNDYING, 1, EFFECTS_DURATION_INFINITE))
            return true;
    } else if (pd->activeItem == itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 1002})) {
        if (_playerUseItemScroll(pd, EFFECT_REGENERATION, 1, 3002))
            return true;
    } else if (pd->activeItem == itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 1003})) {
        if (_playerUseItemScroll(pd, EFFECT_SPEED, 1, 3600 * 2))
            return true;
    } else if (pd->activeItem == itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 1004})) {
        if (_playerUseItemScroll(pd, EFFECT_STRENGTH, 1, 3600 * 4))
            return true;
    } else if (pd->activeItem == itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 1005})) {
        if (_playerUseItemScroll(pd, EFFECT_SHIELDING, 1, 3600 * 4))
            return true;
    } else if (pd->activeItem == itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 1006})) {
        if (_playerUseItemScroll(pd, EFFECT_NIGHTVISION, 1, 3600 * 8))
            return true;
    }

    if (isItemEmpty(pd->activeItem)) {
        removeItemFromInventory(pd->activeItem->slotNum, &(pd->inventory));
        pd->activeItem = &noItem;
    }

    return false;
}

bool playerInteract(PlayerData *pd, int x0, int y0, int x1, int y1) {
    Entity *es[eManager.lastSlot[pd->entity.level]];
    int eSize = getEntities(es, pd->entity.level, x0, y0, x1, y1);
    int i;
    for (i = 0; i < eSize; ++i) {
        Entity *ent = es[i];
        if (ent != &(pd->entity)) {
            if (ItemVsEntity(pd, pd->activeItem, ent, pd->entity.p.dir))
                return true;
        }
    }
    return false;
}

void playerAttack(PlayerData *pd) {
    bool done = false;
    pd->entity.p.attackTimer = 5;
    int yo = -2;
    int range = 12;

    // directly using an item
    if (playerUseItem(pd)) {
        if (pd->activeItem != &noItem && isItemEmpty(pd->activeItem)) {
            removeItemFromInventory(pd->activeItem->slotNum, &(pd->inventory));
            pd->activeItem = &noItem;
        }
        return;
    }

    // interacting with entities
    switch (pd->entity.p.dir) {
    case 0:
        if (playerInteract(pd, pd->entity.x - 8, pd->entity.y + 4 + yo, pd->entity.x + 8, pd->entity.y + range + yo))
            return;
        break;
    case 1:
        if (playerInteract(pd, pd->entity.x - 8, pd->entity.y - range + yo, pd->entity.x + 8, pd->entity.y - 4 + yo))
            return;
        break;
    case 2:
        if (playerInteract(pd, pd->entity.x - range, pd->entity.y - 8 + yo, pd->entity.x - 4, pd->entity.y + 8 + yo))
            return;
        break;
    case 3:
        if (playerInteract(pd, pd->entity.x + 4, pd->entity.y - 8 + yo, pd->entity.x + range, pd->entity.y + 8 + yo))
            return;
        break;
    }

    int xt = pd->entity.x >> 4;
    int yt = (pd->entity.y + yo) >> 4;
    int r = 12;
    if (pd->entity.p.dir == 0)
        yt = (pd->entity.y + r + yo) >> 4;
    if (pd->entity.p.dir == 1)
        yt = (pd->entity.y - r + yo) >> 4;
    if (pd->entity.p.dir == 2)
        xt = (pd->entity.x - r) >> 4;
    if (pd->entity.p.dir == 3)
        xt = (pd->entity.x + r) >> 4;

    // interacting with tiles
    if (xt >= 0 && yt >= 0 && xt < 128 && yt < 128) {
        int itract = itemTileInteract(getTile(pd->entity.level, xt, yt), pd, pd->activeItem, pd->entity.level, xt, yt, pd->entity.x, pd->entity.y, pd->entity.p.dir);
        if (itract > 0) {
            if (itract == 2)
                pd->entity.p.isCarrying = false;
            done = true;
        }

        if (pd->activeItem != &noItem && isItemEmpty(pd->activeItem)) {
            removeItemFromInventory(pd->activeItem->slotNum, &(pd->inventory));
            pd->activeItem = &noItem;
        }
    }

    if (done)
        return;

    // breaking tiles
    if (pd->activeItem == &noItem || pd->activeItem->id == itemGetLegacyId((ItemID){ITEM_CATEGORY_TOOL, 2}) || pd->activeItem->id == itemGetLegacyId((ItemID){ITEM_CATEGORY_FURNITURE, 4})) {
        if (xt >= 0 && yt >= 0 && xt < 128 && 128) {
            playerHurtTile(pd, getTile(pd->entity.level, xt, yt), pd->entity.level, xt, yt, (syncRand() % 3) + 1, pd->entity.p.dir);
        }
    }
}

bool playerUseArea(PlayerData *pd, int x0, int y0, int x1, int y1) {
    Entity *entities[eManager.lastSlot[pd->entity.level]];
    int i;
    int ae = getEntities(entities, pd->entity.level, x0, y0, x1, y1);
    for (i = 0; i < ae; ++i) {
        if (useEntity(pd, entities[i]))
            return true;
    }
    return false;
}

bool playerUse(PlayerData *pd) {
    int yo = -2;
    if (pd->entity.p.dir == 0 && playerUseArea(pd, pd->entity.x - 8, pd->entity.y + 4 + yo, pd->entity.x + 8, pd->entity.y + 12 + yo))
        return true;
    if (pd->entity.p.dir == 1 && playerUseArea(pd, pd->entity.x - 8, pd->entity.y - 12 + yo, pd->entity.x + 8, pd->entity.y - 4 + yo))
        return true;
    if (pd->entity.p.dir == 3 && playerUseArea(pd, pd->entity.x + 4, pd->entity.y - 8 + yo, pd->entity.x + 12, pd->entity.y + 8 + yo))
        return true;
    if (pd->entity.p.dir == 2 && playerUseArea(pd, pd->entity.x - 12, pd->entity.y - 8 + yo, pd->entity.x - 4, pd->entity.y + 8 + yo))
        return true;
    return false;
}

void tickPlayer(PlayerData *pd, bool inmenu) {
    if (pd->entity.p.isDead)
        return;

    playerEffectsUpdate(pd);

    // regeneration
    if (playerEffectActive(pd, EFFECT_REGENERATION)) {
        if (playerEffectGetTime(pd, EFFECT_REGENERATION) % (60 * 10 / playerEffectGetLevel(pd, EFFECT_REGENERATION)) == 1) {
            playerHeal(pd, 1);
        }
    }

    // invincibility time
    if (pd->entity.hurtTime > 0)
        pd->entity.hurtTime--;

    // stamina recharging
    bool swimming = isWater(pd->entity.level, pd->entity.x >> 4, pd->entity.y >> 4);
    if (pd->entity.p.stamina <= 0 && pd->entity.p.staminaRechargeDelay == 0 && pd->entity.p.staminaRecharge == 0) {
        pd->entity.p.staminaRechargeDelay = 40;
    }

    if (pd->entity.p.staminaRechargeDelay > 0) {
        --pd->entity.p.staminaRechargeDelay;
    }

    if (pd->entity.p.staminaRechargeDelay == 0) {
        ++pd->entity.p.staminaRecharge;
        if (swimming)
            pd->entity.p.staminaRecharge = 0;

        while (pd->entity.p.staminaRecharge > 10) {
            pd->entity.p.staminaRecharge -= 10;
            if (pd->entity.p.stamina < 10)
                ++pd->entity.p.stamina;
        }
    }

    if (!inmenu) {
        if (!pd->sprite.choosen) {
            pd->ingameMenu = MENU_CHARACTER_CUSTOMIZE;
            pd->ingameMenuSelection = 0;
            return;
        }

        // movement
        pd->entity.p.ax = 0;
        pd->entity.p.ay = 0;

        int moveSpeed = 1;
        if (playerEffectActive(pd, EFFECT_SPEED)) {
            moveSpeed += playerEffectGetLevel(pd, EFFECT_SPEED);
        }

        if (pd->inputs.k_left.down) {
            pd->entity.p.ax -= moveSpeed;
            pd->entity.p.dir = 2;
            pd->entity.p.walkDist += moveSpeed;
        }
        if (pd->inputs.k_right.down) {
            pd->entity.p.ax += moveSpeed;
            pd->entity.p.dir = 3;
            pd->entity.p.walkDist += moveSpeed;
        }
        if (pd->inputs.k_up.down) {
            pd->entity.p.ay -= moveSpeed;
            pd->entity.p.dir = 1;
            pd->entity.p.walkDist += moveSpeed;
        }
        if (pd->inputs.k_down.down) {
            pd->entity.p.ay += moveSpeed;
            pd->entity.p.dir = 0;
            pd->entity.p.walkDist += moveSpeed;
        }
        if (pd->entity.p.staminaRechargeDelay % 2 == 0)
            moveMob(&(pd->entity), pd->entity.p.ax, pd->entity.p.ay);

        //"pausing", TODO: since multiplayer this will no longer pause
        if (pd->inputs.k_pause.clicked) {
            pd->ingameMenuSelection = 0;
            pd->ingameMenu = MENU_PAUSED;
        }

        // attacking
        if (pd->inputs.k_attack.clicked) {
            if (pd->entity.p.stamina != 0) {
                if (!TESTGODMODE)
                    pd->entity.p.stamina--;
                pd->entity.p.staminaRecharge = 0;

                playerAttack(pd);
            }
        }

        if (pd->inputs.k_menu.clicked) {
            pd->ingameMenuInvSel = 0;
            pd->ingameMenuSelection = 0;
            if (!playerUse(pd))
                pd->ingameMenu = MENU_INVENTORY;
        }
    }

    // swimming stamina and drowning
    if (swimming && pd->entity.p.swimTimer % 60 == 0) {
        if (pd->entity.p.stamina > 0) {
            if (!TESTGODMODE)
                --pd->entity.p.stamina;
        } else {
            hurtEntity(&(pd->entity), 1, -1, 0xFFAF00FF, NULL);
        }
    }

    if (isWater(pd->entity.level, pd->entity.x >> 4, pd->entity.y >> 4))
        ++pd->entity.p.swimTimer;
    if (pd->entity.p.attackTimer > 0)
        --pd->entity.p.attackTimer;

    // TODO - maybe move to own function
    // Update Minimap
    int xp;
    int yp;
    for (xp = (pd->entity.x >> 4) - 5; xp < (pd->entity.x >> 4) + 5; ++xp) {
        for (yp = (pd->entity.y >> 4) - 5; yp < (pd->entity.y >> 4) + 5; ++yp) {
            if (xp >= 0 && xp < 128 && yp >= 0 && yp < 128) {
                if (!getMinimapVisible(pd, pd->entity.level, xp, yp)) {
                    setMinimapVisible(pd, pd->entity.level, xp, yp, true);
                }
            }
        }
    }
}

void playerSetActiveItem(PlayerData *pd, Item *item) {
    pd->activeItem = item;
    if (pd->activeItem->id > 27 && pd->activeItem->id < 51)
        pd->entity.p.isCarrying = true;
    else
        pd->entity.p.isCarrying = false;
}

bool playerUseEnergy(PlayerData *pd, int amount) {
    if (TESTGODMODE)
        return true;
    if (amount > pd->entity.p.stamina)
        return false;
    pd->entity.p.stamina -= amount;
    return true;
}

void playerHeal(PlayerData *pd, int amount) {
    pd->entity.p.health += amount;
    if (pd->entity.p.health > 10)
        pd->entity.p.health = 10;

    char healText[11];
    sprintf(healText, "%d", amount);
    addEntityToList(newParticleText(healText, 0xFF00FF00, pd->entity.x, pd->entity.y, pd->entity.level), &eManager);
}

void playerDamage(PlayerData *pd, int damage, int dir, MColor hurtColor, Entity *damager) {
    if (TESTGODMODE)
        return;
    if (pd->entity.hurtTime > 0)
        return;

    // damage reducing effects
    if (playerEffectActive(pd, EFFECT_SHIELDING)) {
        uByte level = playerEffectGetLevel(pd, EFFECT_SHIELDING);
        damage -= level;
    }
    if (damage <= 0)
        return;

    // damage player
    pd->entity.p.health -= damage;
    pd->entity.hurtTime = 10;

    // player death
    if (pd->entity.p.health < 1) {
        if (playerEffectActive(pd, EFFECT_UNDYING)) {
            pd->entity.p.health = 10;
            playerEffectRemove(pd, EFFECT_UNDYING);
        } else {
            playSoundPositioned(snd_bossdeath, pd->entity.level, pd->entity.x, pd->entity.y);
            pd->entity.p.endTimer = 60;
            pd->entity.p.isDead = true;

            // TODO: Drop items?
        }
    }

    // create effects
    playSoundPositioned(snd_monsterHurt, pd->entity.level, pd->entity.x, pd->entity.y);

    char hurtText[11];
    sprintf(hurtText, "%d", damage);
    addEntityToList(newParticleText(hurtText, hurtColor, pd->entity.x, pd->entity.y, pd->entity.level), &eManager);

    // knockback
    switch (dir) {
    case -1:
        switch (pd->entity.p.dir) {
        case 0:
            pd->entity.yKnockback = -10;
            break;
        case 1:
            pd->entity.yKnockback = +10;
            break;
        case 2:
            pd->entity.xKnockback = +10;
            break;
        case 3:
            pd->entity.xKnockback = -10;
            break;
        }
        break;
        break;
    case 0:
        pd->entity.yKnockback = +10;
        break;
    case 1:
        pd->entity.yKnockback = -10;
        break;
    case 2:
        pd->entity.xKnockback = -10;
        break;
    case 3:
        pd->entity.xKnockback = +10;
        break;
    }
}

void playerSpawn(PlayerData *pd) {
    while (true) {
        int rx = syncRand() % 128;
        int ry = syncRand() % 128;
        if (getTile(pd->entity.level, rx, ry) == TILE_GRASS) {
            pd->entity.x = (rx << 4) + 8;
            pd->entity.y = (ry << 4) + 8;
            pd->isSpawned = true;
            break;
        }
    }
}

// effects
void playerEffectsUpdate(PlayerData *pd) {
    int i;
    for (i = 0; i < EFFECTS_MAX; i++) {
        // if effect is active and not infinite
        if (pd->effects[i].level != 0) {
            if (pd->effects[i].time != EFFECTS_DURATION_INFINITE) {
                pd->effects[i].time--;

                // remove effect if time ran out
                if (pd->effects[i].time == 0) {
                    pd->effects[i].level = 0;
                }
            }
        }
    }
}

bool playerEffectActive(PlayerData *pd, int effect) {
    return pd->effects[effect].level != 0;
}

void playerEffectApply(PlayerData *pd, int effect, uByte level, sInt time) {
    if (level == 0 || time == 0)
        return;

    bool doApply = false;
    if (playerEffectActive(pd, effect)) {
        if (pd->effects[effect].level < level || pd->effects[effect].time < time) {
            doApply = true;
        }
    } else {
        doApply = true;
    }

    if (doApply) {
        pd->effects[effect].level = level;
        pd->effects[effect].time = time;
    }
}

void playerEffectRemove(PlayerData *pd, int effect) {
    pd->effects[effect].level = 0;
    pd->effects[effect].time = 0;
}

uByte playerEffectGetLevel(PlayerData *pd, int effect) {
    return pd->effects[effect].level;
}

sInt playerEffectGetTime(PlayerData *pd, int effect) {
    return pd->effects[effect].time;
}
