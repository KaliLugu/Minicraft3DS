#include <3ds.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
#include "network/Network.h"
#include "network/PacketHandler.h"
#include "network/Synchronizer.h"
#include "render/TextureManager.h"
#include "texturepack.h"

// TODO: Dungeon is way to difficult
//       -> Skeleton arrows are slower, do a little less damage
//       -> Or instead of less damage, implement a simple armor system

// TODO: Something still causes desyncs very rarely

void setupGame() {
    synchronizerInit(rand(), 1, 0);
    synchronizerSetPlayerUID(0, localUID);
    synchronizerStart();

    initGame = 0;
}

void setupGameServer() {
    size_t size;

    networkHostStopConnections();

    networkStart();

    // send start info (seed)
    size = writeStartPacket(networkWriteBuffer, rand());
    networkSend(networkWriteBuffer, size);
    processPacket(networkWriteBuffer, size);
    networkSendWaitFlush();

    // send save file if loading
    FILE *file = fopen(currentFileName, "rb");
    if (file != NULL) {
        sendFile(file, 0, 0);
        networkSendWaitFlush();
        fclose(file);
    }

    // send start command
    size = writeStartRequestPacket(networkWriteBuffer);
    networkSend(networkWriteBuffer, size);
    processPacket(networkWriteBuffer, size);

    initMPGame = 0;
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
    cfguInit();
    CFGU_GetSystemModel(&MODEL_3DS);
    FILE *file;
    shouldRenderDebug = false;
    if ((file = fopen("settings.bin", "r"))) {
        fread(&shouldRenderDebug, sizeof(bool), 1, file);
        fread(&shouldSpeedup, sizeof(bool), 1, file);
        osSetSpeedupEnable(shouldSpeedup);
        fclose(file);
    }

    initEngine(&draw);

    networkInit();
    romfsInit();

    srand(time(NULL));

    // load or create localUID
    if ((file = fopen("m3ds_uid.bin", "rb"))) {
        fread(&localUID, sizeof(u32), 1, file);
        fclose(file);
    } else {
        localUID = (((u32)(rand() % 256)) << 24) | (((u32)(rand() % 256)) << 16) | (((u32)(rand() % 256)) << 8) | (((u32)(rand() % 256)));

        if ((file = fopen("m3ds_uid.bin", "wb"))) {
            fwrite(&localUID, sizeof(u32), 1, file);
            fclose(file);
        }
    }

    noItem = newItem(ITEM_NULL, 0);

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
    localInputs.k_up.input = KEY_DUP | KEY_CPAD_UP | KEY_CSTICK_UP;
    localInputs.k_down.input = KEY_DDOWN | KEY_CPAD_DOWN | KEY_CSTICK_DOWN;
    localInputs.k_left.input = KEY_DLEFT | KEY_CPAD_LEFT | KEY_CSTICK_LEFT;
    localInputs.k_right.input = KEY_DRIGHT | KEY_CPAD_RIGHT | KEY_CSTICK_RIGHT;
    localInputs.k_attack.input = KEY_A | KEY_B | KEY_L | KEY_ZR;
    localInputs.k_menu.input = KEY_X | KEY_Y | KEY_R | KEY_ZL;
    localInputs.k_pause.input = KEY_START;
    localInputs.k_accept.input = KEY_A;
    localInputs.k_decline.input = KEY_B;
    localInputs.k_delete.input = KEY_X;
    localInputs.k_menuNext.input = KEY_R;
    localInputs.k_menuPrev.input = KEY_L;

    /* If btnSave exists, then use that. */
    if ((file = fopen("btnSave.bin", "rb"))) {
        fread(&(localInputs.k_up.input), sizeof(int), 1, file);
        fread(&(localInputs.k_down.input), sizeof(int), 1, file);
        fread(&(localInputs.k_left.input), sizeof(int), 1, file);
        fread(&(localInputs.k_right.input), sizeof(int), 1, file);
        fread(&(localInputs.k_attack.input), sizeof(int), 1, file);
        fread(&(localInputs.k_menu.input), sizeof(int), 1, file);
        fread(&(localInputs.k_pause.input), sizeof(int), 1, file);
        fread(&(localInputs.k_accept.input), sizeof(int), 1, file);
        fread(&(localInputs.k_decline.input), sizeof(int), 1, file);
        fread(&(localInputs.k_delete.input), sizeof(int), 1, file);
        fread(&(localInputs.k_menuNext.input), sizeof(int), 1, file);
        fread(&(localInputs.k_menuPrev.input), sizeof(int), 1, file);
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
    while (aptMainLoop()) {
        if (quitGame)
            break;

        if (initGame > 0 && --initGame == 0)
            setupGame();
        if (initMPGame > 0 && --initMPGame == 0)
            setupGameServer();
        if (initBGMap > 0 && --initBGMap == 0)
            setupBGMap();

        if (currentMenu == MENU_NONE) {
            tickGame();
        } else {
            // input scanning ingame is handled by the synchronizer
            hidScanInput();
            tickKeys(&localInputs, hidKeysHeld(), hidKeysDown());

            tickMenu(currentMenu);
        }

        drawGraphics();
    }

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

    romfsExit();
    networkExit();
    exitEngine();
    cfguExit();
    return 0;
}
