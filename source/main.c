#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#include "Data.h"
#include "Globals.h"
#include "Ingame.h"
#include "MapGen.h"
#include "Menu.h"
#include "Player.h"
#include "Render.h"
#include "SaveLoad.h"
#include "engine/engine.h"
#include "network/RP2P.h"
#include "network/Synchronizer.h"
#include "render/TextureManager.h"
#include "data/items/ItemsData.h"
#include "texturepack.h"

// TODO: Dungeon is way to difficult
//       -> Skeleton arrows are slower, do a little less damage
//       -> Or instead of less damage, implement a simple armor system

// TODO: Something still causes desyncs very rarely

void setupGame() {
    initGame = 0;
    synchronizerStartSP();
}

void setupGameServer() {
    initMPGame = 0;
    networkHostStopConnections();
    synchronizerStartMPHost();
}

void setupBGMap() {
    // Reset entity manager.
    memset(&eManager, 0, sizeof(eManager));
    setClearColor(0xFF6C6D82);

    srand(time(NULL));
    createAndValidateTopMap(128, 128, 1, worldData.map[1], worldData.data[1]);

    // Reset entity manager.
    memset(&eManager, 0, sizeof(eManager));
    setClearColor(0xFF6C6D82);

    initBGMap = 0;
}

bool tick() {
    if (quitGame)
        return false;

    if (initGame > 0 && --initGame == 0)
        setupGame();
    if (initMPGame > 0 && --initMPGame == 0)
        setupGameServer();
    if (initBGMap > 0 && --initBGMap == 0)
        setupBGMap();

    rp2pTick();

    if (currentMenu == MENU_NONE) {
        tickGame();
    } else {
        // input scanning ingame is handled by the synchronizer
        scanInputs();
        tickKeys(&localInputs);

        tickMenu(currentMenu);
    }

    return true;
}

void draw(int screen, int width, int height) {
    if (currentMenu == MENU_NONE) {
        renderGame(screen, width, height);
    } else {
        renderMenu(currentMenu, screen, width, height);
    }
}

char debugText[34];
char bossHealthText[34];
int main() {
    initEngine(&rp2pRecvRaw);

    // store all files in own directory in sdcard
    struct stat st = {0};
    if (stat("/minicraft", &st) == -1) {
        mkdir("/minicraft", 0777);
    }
    chdir("minicraft/");

    FILE *file;
    shouldRenderDebug = false;
    if ((file = fopen("settings.bin", "r"))) {
        fread(&shouldRenderDebug, sizeof(bool), 1, file);
        bool speedUp = false;
        fread(&speedUp, sizeof(bool), 1, file);
        setSpeedUp(speedUp);
        fclose(file);
    }

    srand(time(NULL));

    // load or create localUID
    if ((file = fopen("m3ds_uid.bin", "rb"))) {
        fread(&localUID, sizeof(sInt), 1, file);
        fclose(file);
    } else {
        do {
            localUID = (((sInt)(rand() % 256)) << 24) | (((sInt)(rand() % 256)) << 16) | (((sInt)(rand() % 256)) << 8) | (((sInt)(rand() % 256)));
        } while (localUID == 0);

        if ((file = fopen("m3ds_uid.bin", "wb"))) {
            fwrite(&localUID, sizeof(sInt), 1, file);
            fclose(file);
        }
    }

    noItem = newItem(itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 0}), 0);

    initMenus();
    currentMenu = MENU_TITLE;
    currentSelection = 0;
    quitGame = false;
    initBGMap = 1;

    loadSounds();
    playMusic(music_menu);

    bakeLights();

    for (int i = 0; i < 6; ++i) {
        imageMinimap[i] = createImage(128, 128);
    }

    /* Default inputs (also located in MenuSettingsRebind.c) */
    localInputs.k_up.input = I_DP_UP | I_SL_UP | I_SR_UP;
    localInputs.k_down.input = I_DP_DOWN | I_SL_DOWN | I_SR_DOWN;
    localInputs.k_left.input = I_DP_LEFT | I_SL_LEFT | I_SR_LEFT;
    localInputs.k_right.input = I_DP_RIGHT | I_SL_RIGHT | I_SR_RIGHT;
    localInputs.k_attack.input = I_A | I_B | I_L | I_ZR;
    localInputs.k_menu.input = I_X | I_Y | I_R | I_ZL;
    localInputs.k_pause.input = I_START_PLUS;
    localInputs.k_accept.input = I_A;
    localInputs.k_decline.input = I_B;
    localInputs.k_delete.input = I_X;
    localInputs.k_menuNext.input = I_R;
    localInputs.k_menuPrev.input = I_L;

    /* If btnSave exists, then use that. */
    if ((file = fopen("btnSave.bin", "rb"))) {
        fread(&(localInputs.k_up.input), sizeof(sInt), 1, file);
        fread(&(localInputs.k_down.input), sizeof(sInt), 1, file);
        fread(&(localInputs.k_left.input), sizeof(sInt), 1, file);
        fread(&(localInputs.k_right.input), sizeof(sInt), 1, file);
        fread(&(localInputs.k_attack.input), sizeof(sInt), 1, file);
        fread(&(localInputs.k_menu.input), sizeof(sInt), 1, file);
        fread(&(localInputs.k_pause.input), sizeof(sInt), 1, file);
        fread(&(localInputs.k_accept.input), sizeof(sInt), 1, file);
        fread(&(localInputs.k_decline.input), sizeof(sInt), 1, file);
        fread(&(localInputs.k_delete.input), sizeof(sInt), 1, file);
        fread(&(localInputs.k_menuNext.input), sizeof(sInt), 1, file);
        fread(&(localInputs.k_menuPrev.input), sizeof(sInt), 1, file);
        fclose(file);
    }

    /* If lastTP exists, then use that. */
    if ((file = fopen("lastTP.bin", "r"))) {
        char fnbuf[256];
        fgets(fnbuf, 256, file); // get directory to texturepack
        loadTexturePack(fnbuf);
        fclose(file);
    } else {
        loadTexturePack(NULL);
    }

    initData();

    initPlayers();
    initRecipes();
    initTrades();

    runMainLoop(&tick, &draw);

    stopMusic();

    freeTrades();
    freeRecipes();
    freePlayers();

    freeLightBakes();
    freeImage(imageIcons);
    freeImage(imagePlayerSprites);
    freeImage(imageFont);
    freeImage(imageBottombg);
    for (int i = 0; i < 6; ++i) {
        freeImage(imageMinimap[i]);
    }

    freeSounds();

    exitEngine();
    return 0;
}
