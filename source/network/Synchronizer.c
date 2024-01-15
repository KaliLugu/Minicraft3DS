#include "Synchronizer.h"

#include "../Globals.h"
#include "../Ingame.h"
#include "../Inputs.h"
#include "../Player.h"
#include "../engine/network.h"
#include "Packet.h"
#include "RP2P.h"
#include <math.h>

#define SYNC_FILE_START 1
#define SYNC_FILE_DATA 2
#define SYNC_START 3
#define SYNC_INPUTS 4

int syncTickCount;

static uByte synchronizerWriteBuffer[NETWORK_MAXDATASIZE + 1];
static FILE *synchronizerRecvFile;
static sInt synchronizerRecvFileSize;

static sInt synchronizerLocalTurn;
static sInt synchronizerNextSeed;
static bool synchronizerRunning = false;

static int synchronizerCurrentTicks;

static void synchronizerHandleStart();
static void synchronizerLoadWorld(sInt seed, uByte initPlayerCount, uByte initPlayerLocalIndex);
static void synchronizerHandleMessage(sInt senderID, uByte senderPlayerIndex, void *message, uShort size);
static void synchronizerReadInputs(void *buffer, uShort size, Inputs *inputs);

static bool synchronizerTurnReady();
static void synchronizerNextTurn();
static int synchronizerGetTurnIndex(sInt turn);

static void synchronizerSendSaveFile();
static void synchronizerSendStart(sInt seed);
static void synchronizerSendLocalInputs();

void synchronizerStartSP() {
    // switch menu
    currentMenu = MENU_LOADING;

    players[0].id = localUID;
    synchronizerLoadWorld(rand(), 1, 0);
}

void synchronizerStartMPHost() {
    rp2pInit(true, localUID, &synchronizerHandleMessage, &synchronizerHandleStart);
}

void synchronizerStartMPClient() {
    rp2pInit(false, localUID, &synchronizerHandleMessage, &synchronizerHandleStart);
}

void synchronizerTick(void (*gtick)(void)) {
    if (synchronizerRunning && synchronizerTurnReady()) {
        synchronizerNextTurn();

        // reset random generators
        srand(synchronizerNextSeed);
        gaussrand(true);

        syncTickCount++;

        // call game tick
        (*gtick)();

        synchronizerNextSeed = rand();
    }
}

void synchronizerReset() {
    synchronizerRunning = false;
    synchronizerCurrentTicks = 1;
    rp2pExit();
    networkDisconnect();
}

bool synchronizerIsRunning() {
    return synchronizerRunning;
}

// helpers for random numbers
// TODO: this is in a wierd place currently
#define PI 3.141592654
double gaussrand(bool reset) {
    static double U, V;
    static int phase = 0;
    double Z;

    if (reset) {
        U = 0;
        V = 0;
        phase = 0;
        return 0;
    }

    if (phase == 0) {
        U = (rand() + 1.) / (RAND_MAX + 2.);
        V = rand() / (RAND_MAX + 1.);
        Z = sqrt(-2 * log(U)) * sin(2 * PI * V);
    } else {
        Z = sqrt(-2 * log(U)) * cos(2 * PI * V);
    }

    phase = 1 - phase;

    return Z;
}

static void synchronizerHandleStart() {
    // switch menu
    currentMenu = MENU_LOADING;

    // host
    if (rp2pGetLocalPlayerIndex() == 0) {
        int seed = rand();
        synchronizerSendSaveFile();
        synchronizerSendStart(seed);

        // start locally
        synchronizerLoadWorld(seed, rp2pGetPlayerCount(), 0);
    }
}

static void synchronizerLoadWorld(sInt seed, uByte initPlayerCount, uByte initPlayerLocalIndex) {
    synchronizerLocalTurn = 0;
    synchronizerNextSeed = seed;
    playerCount = initPlayerCount;
    playerLocalIndex = initPlayerLocalIndex;
    syncTickCount = 0;

    // reset player turn states (e.g. is turn data recieved), first turn needs to happen with no inputs
    for (int i = 0; i < playerCount; i++) {
        resetKeys(&(players[i].inputs));
        for (int j = 0; j < MAX_INPUT_BUFFER; j++) {
            resetKeys(&(players[i].nextInputs[j]));
        }
        players[i].nextTurnReady[0] = true;
        if (rp2pIsActive())
            players[i].id = rp2pGetPlayerID(i);
    }

    // check if save file is present
    bool doLoad = false;
    char *loadName = NULL;

    if (playerLocalIndex == 0) {
        // host and single player need to load the actual file
        FILE *file = fopen(currentFileName, "rb");
        if (file != NULL) {
            fclose(file);
            doLoad = true;
            loadName = currentFileName;
        }
    } else {
        // all others the transfered one
        FILE *file = fopen("tmpTransfer.bin", "rb");
        if (file != NULL) {
            fclose(file);
            doLoad = true;
            loadName = "tmpTransfer.bin";
        }
    }

    // reset random generators
    srand(synchronizerNextSeed);
    gaussrand(true);

    // start the game
    startGame(doLoad, loadName);

    // remove transfered save file from clients
    if (playerLocalIndex != 0) {
        FILE *file = fopen("tmpTransfer.bin", "rb");
        if (file != NULL) {
            fclose(file);
            remove("tmpTransfer.bin");
        }
    }

    // clear menu
    currentMenu = MENU_NONE;

    synchronizerCurrentTicks = 1;
    synchronizerRunning = true;
}

static void synchronizerHandleMessage(sInt senderID, uByte senderPlayerIndex, void *message, uShort size) {
    uByte type;
    message = readUByte(message, &size, &type);

    if (type == SYNC_FILE_START) {
        sInt fsize;
        message = readSInt(message, &size, &fsize);

        synchronizerRecvFile = fopen("tmpTransfer.bin", "wb");
        synchronizerRecvFileSize = fsize;
    } else if (type == SYNC_FILE_DATA) {
        fwrite(message, 1, size, synchronizerRecvFile);

        synchronizerRecvFileSize -= size;
        if (synchronizerRecvFileSize <= 0) {
            fclose(synchronizerRecvFile);
            synchronizerRecvFile = NULL;
        }
    } else if (type == SYNC_START) {
        sInt seed;
        message = readSInt(message, &size, &seed);
        synchronizerLoadWorld(seed, rp2pGetPlayerCount(), rp2pGetLocalPlayerIndex());
    } else if (type == SYNC_INPUTS) {
        sInt turnNumber;
        message = readSInt(message, &size, &turnNumber);

        if (turnNumber >= synchronizerLocalTurn && turnNumber < synchronizerLocalTurn + MAX_INPUT_BUFFER && senderPlayerIndex < playerCount) {
            synchronizerReadInputs(message, size, &(players[senderPlayerIndex].nextInputs[synchronizerGetTurnIndex(turnNumber)]));
            players[senderPlayerIndex].nextTurnReady[synchronizerGetTurnIndex(turnNumber)] = true;
        }
    }
}

static void synchronizerReadInputs(void *buffer, uShort size, Inputs *inputs) {
    buffer = readSInt(buffer, &size, &(inputs->k_touchX));
    buffer = readSInt(buffer, &size, &(inputs->k_touchY));

    buffer = readBool(buffer, &size, &(inputs->k_up.down));
    buffer = readBool(buffer, &size, &(inputs->k_up.clicked));
    buffer = readBool(buffer, &size, &(inputs->k_down.down));
    buffer = readBool(buffer, &size, &(inputs->k_down.clicked));
    buffer = readBool(buffer, &size, &(inputs->k_left.down));
    buffer = readBool(buffer, &size, &(inputs->k_left.clicked));
    buffer = readBool(buffer, &size, &(inputs->k_right.down));
    buffer = readBool(buffer, &size, &(inputs->k_right.clicked));
    buffer = readBool(buffer, &size, &(inputs->k_attack.down));
    buffer = readBool(buffer, &size, &(inputs->k_attack.clicked));
    buffer = readBool(buffer, &size, &(inputs->k_menu.down));
    buffer = readBool(buffer, &size, &(inputs->k_menu.clicked));
    buffer = readBool(buffer, &size, &(inputs->k_pause.down));
    buffer = readBool(buffer, &size, &(inputs->k_pause.clicked));
    buffer = readBool(buffer, &size, &(inputs->k_accept.down));
    buffer = readBool(buffer, &size, &(inputs->k_accept.clicked));
    buffer = readBool(buffer, &size, &(inputs->k_decline.down));
    buffer = readBool(buffer, &size, &(inputs->k_decline.clicked));
    buffer = readBool(buffer, &size, &(inputs->k_delete.down));
    buffer = readBool(buffer, &size, &(inputs->k_delete.clicked));
    buffer = readBool(buffer, &size, &(inputs->k_menuNext.down));
    buffer = readBool(buffer, &size, &(inputs->k_menuNext.clicked));
    buffer = readBool(buffer, &size, &(inputs->k_menuPrev.down));
    buffer = readBool(buffer, &size, &(inputs->k_menuPrev.clicked));
}

// Test if all players (including single player) input is recieved
static bool synchronizerTurnReady() {
    if (synchronizerCurrentTicks < SYNCHRONIZER_TICKS_PER_TURN)
        return true;

    for (int i = 0; i < playerCount; i++) {
        if (!players[i].nextTurnReady[synchronizerGetTurnIndex(synchronizerLocalTurn)]) {
            return false;
        }
    }

    return true;
}

static void synchronizerNextTurn() {
    if (synchronizerCurrentTicks < SYNCHRONIZER_TICKS_PER_TURN) {
        synchronizerCurrentTicks++;

        // clicked events are only fired for the first tick per turn
        for (int i = 0; i < playerCount; i++) {
            resetClicked(&(players[i].inputs));
        }
    } else {
        // move nextInput of every player to currentInput
        for (int i = 0; i < playerCount; i++) {
            players[i].inputs = players[i].nextInputs[synchronizerGetTurnIndex(synchronizerLocalTurn)];
            players[i].nextTurnReady[synchronizerGetTurnIndex(synchronizerLocalTurn)] = false;
        }

        // Increase turn number
        synchronizerLocalTurn++;
        synchronizerCurrentTicks = 1;

        // send local input
        synchronizerSendLocalInputs();
    }
}

static int synchronizerGetTurnIndex(sInt turn) {
    return turn % MAX_INPUT_BUFFER;
}

static void synchronizerSendLocalInputs() {
    // scan local inputs
    scanInputs();
    tickKeys(&localInputs);

    // store local input in nextInput
    players[playerLocalIndex].nextInputs[synchronizerGetTurnIndex(synchronizerLocalTurn)] = localInputs;
    players[playerLocalIndex].nextTurnReady[synchronizerGetTurnIndex(synchronizerLocalTurn)] = true;

    // send local input
    if (rp2pIsActive()) {
        void *buffer = synchronizerWriteBuffer;
        uShort size = 0;
        buffer = writeUByte(buffer, &size, SYNC_INPUTS);
        buffer = writeSInt(buffer, &size, synchronizerLocalTurn);

        buffer = writeSInt(buffer, &size, localInputs.k_touchX);
        buffer = writeSInt(buffer, &size, localInputs.k_touchY);

        buffer = writeBool(buffer, &size, localInputs.k_up.down);
        buffer = writeBool(buffer, &size, localInputs.k_up.clicked);
        buffer = writeBool(buffer, &size, localInputs.k_down.down);
        buffer = writeBool(buffer, &size, localInputs.k_down.clicked);
        buffer = writeBool(buffer, &size, localInputs.k_left.down);
        buffer = writeBool(buffer, &size, localInputs.k_left.clicked);
        buffer = writeBool(buffer, &size, localInputs.k_right.down);
        buffer = writeBool(buffer, &size, localInputs.k_right.clicked);
        buffer = writeBool(buffer, &size, localInputs.k_attack.down);
        buffer = writeBool(buffer, &size, localInputs.k_attack.clicked);
        buffer = writeBool(buffer, &size, localInputs.k_menu.down);
        buffer = writeBool(buffer, &size, localInputs.k_menu.clicked);
        buffer = writeBool(buffer, &size, localInputs.k_pause.down);
        buffer = writeBool(buffer, &size, localInputs.k_pause.clicked);
        buffer = writeBool(buffer, &size, localInputs.k_accept.down);
        buffer = writeBool(buffer, &size, localInputs.k_accept.clicked);
        buffer = writeBool(buffer, &size, localInputs.k_decline.down);
        buffer = writeBool(buffer, &size, localInputs.k_decline.clicked);
        buffer = writeBool(buffer, &size, localInputs.k_delete.down);
        buffer = writeBool(buffer, &size, localInputs.k_delete.clicked);
        buffer = writeBool(buffer, &size, localInputs.k_menuNext.down);
        buffer = writeBool(buffer, &size, localInputs.k_menuNext.clicked);
        buffer = writeBool(buffer, &size, localInputs.k_menuPrev.down);
        buffer = writeBool(buffer, &size, localInputs.k_menuPrev.clicked);
        rp2pSend(synchronizerWriteBuffer, size);
    }
}

static void synchronizerSendSaveFile() {
    FILE *file = fopen(currentFileName, "rb");
    if (file != NULL) {
        fseek(file, 0, SEEK_END); // seek to end of file
        sInt fsize = ftell(file); // get current file pointer
        fseek(file, 0, SEEK_SET); // seek back to beginning of file

        // send file header
        void *buffer = synchronizerWriteBuffer;
        uShort size = 0;
        buffer = writeUByte(buffer, &size, SYNC_FILE_START);
        buffer = writeSInt(buffer, &size, fsize);
        rp2pSend(synchronizerWriteBuffer, size);

        // send file data
        while (fsize > 0) {
            buffer = synchronizerWriteBuffer;
            size = 0;
            buffer = writeUByte(buffer, &size, SYNC_FILE_DATA);

            // read file data
            sShort towrite = NETWORK_MAXDATASIZE - size;
            if (towrite > fsize)
                towrite = fsize;

            fread(buffer, 1, towrite, file);

            size += towrite;
            fsize -= towrite;

            // send file data
            rp2pSend(synchronizerWriteBuffer, size);
        }
        rp2pFlush();
        fclose(file);
    }
}

static void synchronizerSendStart(sInt seed) {
    void *buffer = synchronizerWriteBuffer;
    uShort size = 0;
    buffer = writeUByte(buffer, &size, SYNC_START);
    buffer = writeSInt(buffer, &size, seed);
    rp2pSend(synchronizerWriteBuffer, size);
    rp2pFlush();
}
