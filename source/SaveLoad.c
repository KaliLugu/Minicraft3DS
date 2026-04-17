#include "data/items/ItemsData.h"

#include "SaveLoad.h"

#include "version.h"
#include "Globals.h"
#include "ZipHelper.h"
#include <ctype.h>
#include <dirent.h>
#include <stdio.h>

sShort calculateImportantEntites(EntityManager *eManager, uByte level) {
    sShort count = 0;
    for (int i = 0; i < eManager->lastSlot[level]; i++) {
        if (persistEntity(&eManager->entities[level][i])) {
            count++;
        }
    }
    return count;
}

// helper methods
char **files;
int fileCount;

void saveTrackFileReset() {
    if (files != NULL) {
        for (int i = 0; i < fileCount; i++) {
            free(files[i]);
        }
        free(files);
    }

    files = NULL;
    fileCount = 0;
}

int saveTrackFile(char *filename) {
    // test if entry allready present
    for (int i = 0; i < fileCount; i++) {
        if (strcmp(filename, files[i]) == 0) {
            return 0;
        }
    }

    // add new entry
    fileCount++;
    char **newFiles = realloc(files, fileCount * sizeof(char *));
    if (!newFiles) {
        for (int i = 0; i < fileCount - 1; i++) {
            free(files[i]);
        }
        free(files);
        files = NULL;
        return 1;
    }

    files = newFiles;
    files[fileCount - 1] = malloc(strlen(filename) + 1);
    strcpy(files[fileCount - 1], filename);
    return 0;
}

void saveDeleteTrackedFiles() {
    for (int i = 0; i < fileCount; i++) {
        remove(files[i]);
    }
}

static bool saveFileCopy(char *target, char *source) {
    char *buffer = malloc(SAVE_COPYBUFFER_SIZE);
    if (buffer == NULL) {
        return false;
    }

    FILE *in = fopen(source, "rb");
    if (in == NULL) {
        free(buffer);
        return false;
    }
    FILE *out = fopen(target, "wb");
    if (out == NULL) {
        fclose(in);
        free(buffer);
        return false;
    }

    size_t size;
    do {
        size = fread(buffer, 1, SAVE_COPYBUFFER_SIZE, in);

        if (size > 0) {
            fwrite(buffer, 1, size, out);
        }
    } while (size > 0);

    fclose(in);
    fclose(out);
    free(buffer);

    return true;
}

extern bool renameWorld(char *oldFilename, char *newFilename) {
    FILE *file = fopen(newFilename, "rb");
    if (file != NULL) {
        fclose(file);
        return false;
    }

    return rename(oldFilename, newFilename) == 0;
}

// TODO CRITICAL CHANGE SIZE OF INVENTORY FOR USE GOOD
// internal save methods
void saveInventory(Inventory *inv, EntityManager *eManager, FILE *file) {
    fwrite(&inv->lastSlot, sizeof(sShort), 1, file); // write amount of items in inventory;
    for (int j = 0; j < inv->lastSlot; ++j) {
        // Get the stable internal name for the item, not the display name.
        const char *name = getNameFromId(inv->items[j].id);
        size_t nameLen = strlen(name);
        // Write name length and name
        fwrite(&nameLen, sizeof(size_t), 1, file);
        fwrite(name, 1, nameLen, file);

        // Save item count/level after the stable item name.
        fwrite(&(inv->items[j].countLevel), sizeof(sShort), 1, file); // write count/level of item
        if (inv->items[j].id == getIdFromName("ITEM_CHEST")) { // chest
            int invIndex = (inv->items[j].chestPtr != NULL)
                ? (int)(inv->items[j].chestPtr - eManager->invs)
                : -1;
            fwrite(&invIndex, sizeof(int), 1, file);
        }
    }
}

void saveEntity(Entity *e, EntityManager *eManager, FILE *file) {
    fwrite(&e->type, sizeof(sShort), 1, file); // write entity's type ID
    fwrite(&e->x, sizeof(sShort), 1, file);    // write entity's x coordinate
    fwrite(&e->y, sizeof(sShort), 1, file);    // write entity's y coordinate
    switch (e->type) {
    case ENTITY_AIRWIZARD:
        fwrite(&e->wizard.health, sizeof(sShort), 1, file);
        break;
    case ENTITY_ZOMBIE:
    case ENTITY_SKELETON:
    case ENTITY_KNIGHT:
    case ENTITY_SLIME:
        fwrite(&e->hostile.health, sizeof(sShort), 1, file);
        fwrite(&e->hostile.lvl, sizeof(sByte), 1, file);
        break;
    case ENTITY_ITEM: {
        const char *name = getNameFromId(e->entityItem.item.id);
        size_t nameLen = strlen(name);
        fwrite(&nameLen, sizeof(size_t), 1, file);
        fwrite(name, 1, nameLen, file);

        fwrite(&e->entityItem.item.countLevel, sizeof(sShort), 1, file);
        fwrite(&e->entityItem.age, sizeof(sShort), 1, file);
        break;
    }
    case ENTITY_FURNITURE: {
        const char *furnitureName = getNameFromId(e->entityFurniture.itemID);
        size_t furnitureNameLen = strlen(furnitureName);
        fwrite(&furnitureNameLen, sizeof(size_t), 1, file);
        fwrite(furnitureName, 1, furnitureNameLen, file);

        int invIndex = (e->entityFurniture.inv != NULL)
            ? (int)(e->entityFurniture.inv - eManager->invs)
            : -1;
        fwrite(&invIndex, sizeof(int), 1, file);
        break;
    }
    case ENTITY_PASSIVE:
        fwrite(&e->passive.health, sizeof(sShort), 1, file);
        fwrite(&e->passive.mtype, sizeof(uByte), 1, file);
        break;
    case ENTITY_DRAGON:
        fwrite(&e->dragon.health, sizeof(sShort), 1, file);
        break;
    case ENTITY_NPC:
        fwrite(&e->npc.type, sizeof(uByte), 1, file);
        break;
    }
}

void saveWorldInternal(char *filename, EntityManager *eManager, WorldData *worldData) {
    FILE *file = fopen(filename, "wb"); // TODO: should be checked

    int i, j;

    // write savefile version
    int version = SAVE_VERSION;
    fwrite(&version, sizeof(int), 1, file);

    // Inventory Data
    fwrite(&eManager->nextInv, sizeof(sShort), 1, file); // write amount of inventories.
    for (i = 0; i < eManager->nextInv; ++i) {
        saveInventory(&(eManager->invs[i]), eManager, file);
    }

    // Entity Data
    for (i = 0; i < 5; ++i) { // for every level (except dungeon of course)
        int amount = calculateImportantEntites(eManager, i);
        fwrite(&amount, sizeof(sShort), 1, file); // read amount of entities in level.
        for (j = 0; j < eManager->lastSlot[i]; ++j) {
            if (!persistEntity(&eManager->entities[i][j]))
                continue;

            saveEntity(&eManager->entities[i][j], eManager, file);
        }
    }

    // Day/season Data
    fwrite(&worldData->daytime, sizeof(uShort), 1, file);
    fwrite(&worldData->day, sizeof(int), 1, file);
    fwrite(&worldData->season, sizeof(uByte), 1, file);
    fwrite(&worldData->rain, sizeof(bool), 1, file);

    // Compass Data
    fwrite(worldData->compassData, sizeof(uByte), 6 * 3, file); // x,y of choosen stair and count per level

    // Map Data
    // Don't write or load dungeon, so only first 5 levels not 6
    fwrite(worldData->map, sizeof(uByte), 128 * 128 * 5, file);  // Map Tile IDs, 128*128*5 bytes = 80KB
    fwrite(worldData->data, sizeof(uByte), 128 * 128 * 5, file); // Map Tile Data (Damage done to trees/rocks, age of wheat & saplings, etc). 80KB

    // write version string
    char versionBuf[16] = {0};
    strncpy(versionBuf, VERSION_STRING, sizeof(versionBuf) - 1);
    fwrite(versionBuf, sizeof(versionBuf), 1, file);

    fclose(file);
}

void savePlayerInternal(char *filename, PlayerData *player, EntityManager *eManager) {
    FILE *file = fopen(filename, "wb"); // TODO: should be checked

    int i;

    // write savefile version
    int version = SAVE_VERSION;
    fwrite(&version, sizeof(int), 1, file);

    // basic player info
    fwrite(&player->score, sizeof(int), 1, file);
    fwrite(&player->isSpawned, sizeof(bool), 1, file);
    fwrite(&player->entity.p.hasWonSaved, sizeof(bool), 1, file);
    fwrite(&player->entity.p.health, sizeof(sShort), 1, file);
    fwrite(&player->entity.x, sizeof(sShort), 1, file);
    fwrite(&player->entity.y, sizeof(sShort), 1, file);
    fwrite(&player->entity.level, sizeof(sByte), 1, file);

    saveInventory(&(player->inventory), eManager, file);

    // Sprite info
    fwrite(&(player->sprite.choosen), sizeof(bool), 1, file);
    fwrite(&(player->sprite.legs), sizeof(uByte), 1, file);
    fwrite(&(player->sprite.body), sizeof(uByte), 1, file);
    fwrite(&(player->sprite.arms), sizeof(uByte), 1, file);
    fwrite(&(player->sprite.head), sizeof(uByte), 1, file);
    fwrite(&(player->sprite.eyes), sizeof(uByte), 1, file);
    fwrite(&(player->sprite.accs), sizeof(uByte), 1, file);

    // Effect Data
    int esize = EFFECTS_MAX;
    fwrite(&esize, sizeof(int), 1, file);
    for (i = 0; i < EFFECTS_MAX; i++) {
        fwrite(&(player->effects[i].level), sizeof(uByte), 1, file);
        fwrite(&(player->effects[i].time), sizeof(sInt), 1, file);
    }

    // Minimap Data
    fwrite(player->minimapData, sizeof(uByte), 128 * 128, file); // Minimap, visibility data 16KB

    // Quest Data
    fwrite(&(player->questManager.size), sizeof(int), 1, file);
    for (i = 0; i < player->questManager.size; ++i) {
        fwrite(&(player->questManager.questlines[i].currentQuest), sizeof(int), 1, file);
        fwrite(&(player->questManager.questlines[i].currentQuestDone), sizeof(bool), 1, file);
    }

    fclose(file);
}

// internal load methods
void loadInventory(Inventory *inv, EntityManager *eManager, FILE *file, int version) {
    fread(&(inv->lastSlot), sizeof(sShort), 1, file); // read amount of items in inventory;
    for (int j = 0; j < inv->lastSlot; ++j) {
        size_t nameLen;
        fread(&nameLen, sizeof(size_t), 1, file);
        
        // Utiliser un tampon temporaire pour éviter les échecs malloc
        char temp_name[64]; // Taille réaliste basée sur les noms d'items (max ~22 chars)
        if (nameLen >= sizeof(temp_name)) {
            // Gestion d'erreur : nom trop long, ignorer l'item
            size_t remaining = nameLen;
            while (remaining > 0) {
                size_t chunk_size = remaining > 1024 ? 1024 : remaining;
                long chunk = (long)chunk_size;
                fseek(file, chunk, SEEK_CUR); // Sauter le nom en chunks sûrs
                remaining -= chunk_size;
            }
            fread(&(inv->items[j].countLevel), sizeof(sShort), 1, file);
            inv->items[j].id = 0; // ID par défaut
            inv->items[j].invPtr = (int *)inv;
            inv->items[j].slotNum = j;
            continue;
        }
        
        fread(temp_name, 1, nameLen, file);
        temp_name[nameLen] = '\0';
        
        // Obtenir l'ID depuis le nom
        inv->items[j].id = getIdFromName(temp_name);
        
        fread(&(inv->items[j].countLevel), sizeof(sShort), 1, file); // read count/level of item
        
        inv->items[j].invPtr = (int *)inv;    // setup Inventory pointer
        inv->items[j].slotNum = j;            // setup slot number
        if (inv->items[j].id == getIdFromName("ITEM_CHEST")) { // for chest item specifically.
            int invIndex;
            fread(&invIndex, sizeof(int), 1, file);
            // Validate invIndex to prevent out-of-bounds access on corrupted save files
            if (invIndex >= 0 && invIndex < eManager->nextInv) {
                inv->items[j].chestPtr = (Inventory *)&eManager->invs[invIndex]; // setup chest inventory pointer.
            } else {
                inv->items[j].chestPtr = NULL;
            }
        }
    }
}

void loadEntity(Entity *e, uByte level, int j, EntityManager *eManager, FILE *file, int version) {
    sShort type;
    sShort x;
    sShort y;

    sShort health;
    int lvl;

    int invIndex;

    size_t nameLen;

    fread(&type, sizeof(sShort), 1, file); // read entity's type ID
    fread(&x, sizeof(sShort), 1, file);    // read entity's x coordinate
    fread(&y, sizeof(sShort), 1, file);    // read entity's y coordinate
    switch (type) {
    case ENTITY_AIRWIZARD:
        *e = newEntityAirWizard(x, y, level);
        fread(&e->wizard.health, sizeof(sShort), 1, file);
        break;
    case ENTITY_SLIME:
        fread(&health, sizeof(sShort), 1, file);
        fread(&lvl, sizeof(sByte), 1, file);
        *e = newEntitySlime(lvl, x, y, level);
        e->hostile.health = health;
        break;
    case ENTITY_ZOMBIE:
        fread(&health, sizeof(sShort), 1, file);
        fread(&lvl, sizeof(sByte), 1, file);
        *e = newEntityZombie(lvl, x, y, level);
        e->hostile.health = health;
        break;
    case ENTITY_SKELETON:
        fread(&health, sizeof(sShort), 1, file);
        fread(&lvl, sizeof(sByte), 1, file);
        *e = newEntitySkeleton(lvl, x, y, level);
        e->hostile.health = health;
        break;
    case ENTITY_KNIGHT:
        fread(&health, sizeof(sShort), 1, file);
        fread(&lvl, sizeof(sByte), 1, file);
        *e = newEntityKnight(lvl, x, y, level);
        e->hostile.health = health;
        break;
    case ENTITY_ITEM:
        fread(&nameLen, sizeof(size_t), 1, file);
        
        // Utiliser un tampon temporaire pour éviter les échecs malloc sur des sauvegardes corrompues
        char temp_name[64]; // Taille réaliste basée sur les noms d'items (max ~22 chars)
        if (nameLen >= sizeof(temp_name)) {
            // Gestion d'erreur : nom trop long, ignorer l'entité
            size_t remaining = nameLen;
            while (remaining > 0) {
                size_t chunk_size = remaining > 1024 ? 1024 : remaining;
                long chunk = (long)chunk_size;
                fseek(file, chunk, SEEK_CUR); // Sauter le nom en chunks sûrs
                remaining -= chunk_size;
            }
            fseek(file, sizeof(sShort), SEEK_CUR); // Sauter countLevel
            fseek(file, sizeof(sShort), SEEK_CUR); // Sauter age
            *e = newEntityItem(newItem(0, 0), x, y, level); // Item NULL par défaut
            break;
        }
        
        fread(temp_name, 1, nameLen, file);
        temp_name[nameLen] = '\0';
        
        sShort itemId = getIdFromName(temp_name);
        *e = newEntityItem(newItem(itemId, 0), x, y, level);
        fread(&e->entityItem.item.countLevel, sizeof(sShort), 1, file);
        fread(&e->entityItem.age, sizeof(sShort), 1, file);
        break;
    case ENTITY_FURNITURE:
        fread(&nameLen, sizeof(size_t), 1, file);
        
        // Utiliser un tampon temporaire pour éviter les échecs malloc sur des sauvegardes corrompues
        char temp_name_fur[64]; // Taille réaliste basée sur les noms d'items (max ~22 chars)
        if (nameLen >= sizeof(temp_name_fur)) {
            // Gestion d'erreur : nom trop long, ignorer l'entité
            size_t remaining = nameLen;
            while (remaining > 0) {
                size_t chunk_size = remaining > 1024 ? 1024 : remaining;
                long chunk = (long)chunk_size;
                fseek(file, chunk, SEEK_CUR); // Sauter le nom en chunks sûrs
                remaining -= chunk_size;
            }
            fread(&invIndex, sizeof(int), 1, file);
            *e = newEntityFurniture(0, NULL, x, y, level); // Item NULL par défaut
            break;
        }
        
        fread(temp_name_fur, 1, nameLen, file);
        temp_name_fur[nameLen] = '\0';
        
        fread(&invIndex, sizeof(int), 1, file);

        sShort Id = getIdFromName(temp_name_fur);
        // Validate invIndex to prevent out-of-bounds access on corrupted save files
        Inventory *invPtr = (invIndex >= 0 && invIndex < eManager->nextInv) ? &eManager->invs[invIndex] : NULL;
        *e = newEntityFurniture(Id, invPtr, x, y, level);
        break;
    case ENTITY_PASSIVE:
        *e = newEntityPassive(0, x, y, level);
        fread(&e->passive.health, sizeof(sShort), 1, file);
        fread(&e->passive.mtype, sizeof(uByte), 1, file);
        break;
    case ENTITY_GLOWWORM:
        *e = newEntityGlowworm(x, y, level);
        break;
    case ENTITY_DRAGON:
        *e = newEntityDragon(x, y, level);
        fread(&e->dragon.health, sizeof(sShort), 1, file);
        break;
    case ENTITY_NPC:
        *e = newEntityNPC(0, x, y, level);
        fread(&e->npc.type, sizeof(uByte), 1, file);
        break;
    }
    e->type = type;
    e->x = x;
    e->y = y;
    e->slotNum = j;
}

int loadWorldInternal(char *filename, EntityManager *eManager, WorldData *worldData) {
    FILE *file = fopen(filename, "rb"); // TODO: should be checked

    int i, j;

    // read savefile version
    int version;
    char savedVersion[16] = {0};
    
    fseek(file, -16, SEEK_END);
    size_t read = fread(savedVersion, sizeof(savedVersion), 1, file);
    fseek(file, 0, SEEK_SET); // retour au début pour lire SAVE_VERSION

    fread(&version, sizeof(int), 1, file);
    if (version != SAVE_VERSION) {
        fclose(file);
        return -1; // format incompatible
    } 
    if (read != 1 || savedVersion[0] == '\0') {
        // debug("No version string found in save file (legacy save).");
        fclose(file);
        return 2;
    }
    if (!isSameVersion(savedVersion)) {
        // debug("Version string mismatch, expected: %s, got: %s.", VERSION_STRING, savedVersion);
        fclose(file);
        return 1;
    }

    // Inventory Data
    fread(&eManager->nextInv, sizeof(sShort), 1, file);
    for (i = 0; i < eManager->nextInv; ++i) {
        loadInventory(&(eManager->invs[i]), eManager, file, version);
    }

    // Entity Data
    for (i = 0; i < 5; ++i) {
        fread(&eManager->lastSlot[i], sizeof(sShort), 1, file); // read amount of entities in level.
        for (j = 0; j < eManager->lastSlot[i]; ++j) {
            loadEntity(&eManager->entities[i][j], i, j, eManager, file, version);
        }
    }

    // Day/season Data
    fread(&worldData->daytime, sizeof(uShort), 1, file);
    fread(&worldData->day, sizeof(int), 1, file);
    fread(&worldData->season, sizeof(uByte), 1, file);
    fread(&worldData->rain, sizeof(bool), 1, file);

    // Compass Data
    fread(worldData->compassData, sizeof(uByte), 6 * 3, file); // x,y of choosen stair and count per level

    // Map Data
    // Don't write or load dungeon, so only first 5 levels not 6
    fread(worldData->map, sizeof(uByte), 128 * 128 * 5, file);  // Map Tile IDs, 128*128*5 bytes = 80KB
    fread(worldData->data, sizeof(uByte), 128 * 128 * 5, file); // Map Tile Data (Damage done to trees/rocks, age of wheat & saplings, etc). 80KB

    fclose(file);
    return 0;
}

int loadPlayerInternal(char *filename, PlayerData *player, EntityManager *eManager) {
    FILE *file = fopen(filename, "rb"); // TODO: should be checked

    int i;

    // read savefile version
    int version;
    fread(&version, sizeof(int), 1, file);
    if (version != SAVE_VERSION) {
        fclose(file);
        return -1; // format incompatible
    }

    // basic player info
    fread(&player->score, sizeof(int), 1, file);
    fread(&player->isSpawned, sizeof(bool), 1, file);
    fread(&player->entity.p.hasWonSaved, sizeof(bool), 1, file);
    fread(&player->entity.p.health, sizeof(sShort), 1, file);
    fread(&player->entity.x, sizeof(sShort), 1, file);
    fread(&player->entity.y, sizeof(sShort), 1, file);
    fread(&player->entity.level, sizeof(sByte), 1, file);

    loadInventory(&(player->inventory), eManager, file, version);

    // Sprite info
    fread(&(player->sprite.choosen), sizeof(bool), 1, file);
    fread(&(player->sprite.legs), sizeof(uByte), 1, file);
    fread(&(player->sprite.body), sizeof(uByte), 1, file);
    fread(&(player->sprite.arms), sizeof(uByte), 1, file);
    fread(&(player->sprite.head), sizeof(uByte), 1, file);
    fread(&(player->sprite.eyes), sizeof(uByte), 1, file);
    fread(&(player->sprite.accs), sizeof(sByte), 1, file);

    // Effect Data
    int esize;
    fread(&esize, sizeof(int), 1, file);
    for (i = 0; i < esize; i++) {
        fread(&(player->effects[i].level), sizeof(uByte), 1, file);
        fread(&(player->effects[i].time), sizeof(sInt), 1, file);
    }

    // Minimap Data
    fread(player->minimapData, sizeof(uByte), 128 * 128, file); // Minimap, visibility data 16KB

    // Quest Data
    fread(&(player->questManager.size), sizeof(int), 1, file);
    for (i = 0; i < player->questManager.size; ++i) {
        fread(&(player->questManager.questlines[i].currentQuest), sizeof(int), 1, file);
        fread(&(player->questManager.questlines[i].currentQuestDone), sizeof(bool), 1, file);
    }

    fclose(file);
    return 0;
}

bool saveWorld(char *filename, EntityManager *eManager, WorldData *worldData, PlayerData *players, int playerCount) {
    // check if old save file exists
    bool exists = false;
    FILE *testFile = fopen(filename, "rb");
    if (testFile) {
        fclose(testFile);
        exists = true;
    }

    saveTrackFileReset();

    if (exists) {
        // create backup copy
        char *filenameBackup = malloc(sizeof(filename) + 4 + 1);
        if (filenameBackup == NULL) {
            return false;
        }
        strcpy(filenameBackup, filename);
        strcat(filenameBackup, ".bak");
        if (!saveFileCopy(filenameBackup, filename)) {
            return false;
        }

        // extract files and keep track of references
        if (unzipAndLoad(filename, &saveTrackFile, SAVE_COMMENT, ZIPHELPER_KEEP_FILES) != 0) {
            saveDeleteTrackedFiles();
            return false;
        }

        remove(filename);
    }

    // save world data
    saveWorldInternal("main.wld", eManager, worldData);
    saveTrackFile("main.wld");

    // save player data of active players
    for (int i = 0; i < playerCount; i++) {
        char playerFilename[50];
        playerFilename[0] = '\0';
        sprintf(playerFilename, "%lu.plr", players[i].id);

        savePlayerInternal(playerFilename, players + i, eManager);
        saveTrackFile(playerFilename);
    }

    // zip all tracked files
    if (zipFiles(filename, files, fileCount, ZIPHELPER_REPLACE, SAVE_COMMENT) != 0) {
        remove(filename);
    }

    saveDeleteTrackedFiles();

    return true;
}

bool loadHadWorld;
EntityManager *loadEManager;
WorldData *loadWorldData;
PlayerData *loadPlayers;
uByte loadPlayerCount;
int lastLoadError = LOAD_ERROR_NONE;

static int loadFile(char *filename) {
    // load world
    if (strcmp(filename, "main.wld") == 0) {
        int result = loadWorldInternal(filename, loadEManager, loadWorldData);
        if (result != 0) {
            // Track the specific error code, in futur use idNewerVersion and idOlderVersion to show more specific error, for now just show a generic version mismatch error for both cases cause the text in menu is big and specific error messages would be too long
            lastLoadError = (result == 2) ? LOAD_ERROR_LEGACY_SAVE : LOAD_ERROR_VERSION_MISMATCH;
        }
        loadHadWorld = (result == 0);
    }

    // load player data of active players
    for (int i = 0; i < loadPlayerCount; i++) {
        char playerFilename[50];
        playerFilename[0] = '\0';
        sprintf(playerFilename, "%lu.plr", loadPlayers[i].id);

        if (strcmp(filename, playerFilename) == 0) {
            loadPlayerInternal(filename, loadPlayers + i, loadEManager);
        }
    }

    return 0;
}

static sInt *stateScore;
static bool *stateWin;
static int loadFileState(char *filename) {
    char playerFilename[50];
    playerFilename[0] = '\0';
    sprintf(playerFilename, "%lu.plr", localUID);

    if (strcmp(filename, playerFilename) == 0) {
        FILE *file = fopen(filename, "rb"); // TODO: should be checked

        // read savefile version
        int version;
        fread(&version, sizeof(int), 1, file);
        if (version != SAVE_VERSION) {
            fclose(file);
            return -1; // format incompatible
        }

        // basic player info
        bool dummy;
        fread(stateScore, sizeof(int), 1, file);
        fread(&dummy, sizeof(bool), 1, file);
        fread(stateWin, sizeof(bool), 1, file);

        fclose(file);
    }

    return 0;
}

bool loadWorld(char *filename, EntityManager *eManager, WorldData *worldData, PlayerData *players, int playerCount) {
    // check if save file exists
    bool exists = false;
    FILE *testFile = fopen(filename, "rb");
    if (testFile) {
        fclose(testFile);
        exists = true;
    }
    if (!exists) {
        lastLoadError = LOAD_ERROR_FILE_MISSING;
        return false;
    }

    loadHadWorld = false;
    loadEManager = eManager;
    loadWorldData = worldData;
    loadPlayers = players;
    loadPlayerCount = playerCount;
    lastLoadError = LOAD_ERROR_NONE;

    // extract files
    int result = unzipAndLoad(filename, &loadFile, SAVE_COMMENT, ZIPHELPER_CLEANUP_FILES);
    if (result != 0) {
        lastLoadError = LOAD_ERROR_ZIP_FAILED;
        FILE *errorfile;
        if ((errorfile = fopen("m3ds_error.bin", "wb"))) {
            fwrite(&result, sizeof(int), 1, errorfile);
            fclose(errorfile);
        }
        return false;
    }

    if (!loadHadWorld) {
        if (lastLoadError == LOAD_ERROR_NONE) {
            lastLoadError = LOAD_ERROR_MISSING_WORLD;
        }
        return false;
    }

    return true;
}

void getWorldPlayerState(char *filename, sInt *score, bool *win) {
    stateScore = score;
    stateWin = win;
    unzipAndLoad(filename, &loadFileState, SAVE_COMMENT, ZIPHELPER_CLEANUP_FILES);
}

bool isWorldNameValid(char *worldName) {
    return checkFileNameForErrors(worldName) == 0;
}

int checkFileNameForErrors(char *filename) {
    int length = strlen(filename);
    if (length < 1)
        return 1; // Error: Length cannot be 0.
    if (length > 64)
        return 4; // Error: Length cannot be over 64
    bool isGood = false;
    for (int i = 0; i < length; ++i) {
        if (isalnum((int)filename[i]))
            isGood = true;
    }
    if (!isGood)
        return 2; // Error: Filename must contain atleast one letter or number.

    DIR *d;
    struct dirent *dir;
    d = opendir(".");
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if (strstr(dir->d_name, ".msv") != NULL) { // Check if filename contains ".msv"
                char cmprFile[256];
                strncpy(cmprFile, dir->d_name, strlen(dir->d_name) - 4);
                if (strncmp(filename, cmprFile, strlen(filename)) == 0)
                    return 3; // Error: Filename cannot already exist.
            }
        }
        closedir(d);
    }

    return 0; // No errors found!
}

int getLastLoadError() {
    return lastLoadError;
}
