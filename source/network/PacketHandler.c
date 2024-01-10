#include "PacketHandler.h"

#include "../engine/network.h"
#include "Synchronizer.h"
#include <stdlib.h>

static FILE *recvFile;
static size_t recvFileSize;

static void *writeBool(void *buffer, size_t *size, bool value) {
    *((bool *)buffer) = value;
    *(size) += sizeof(bool);
    return buffer + sizeof(bool);
}

static void *writeUByte(void *buffer, size_t *size, uByte value) {
    *((uByte *)buffer) = value;
    *(size) += sizeof(uByte);
    return buffer + sizeof(uByte);
}

static void *writeUShort(void *buffer, size_t *size, uShort value) {
    *((uShort *)buffer) = value;
    *(size) += sizeof(uShort);
    return buffer + sizeof(uShort);
}

static void *writeSInt(void *buffer, size_t *size, sInt value) {
    *((sInt *)buffer) = value;
    *(size) += sizeof(sInt);
    return buffer + sizeof(sInt);
}

static void *writeSizeT(void *buffer, size_t *size, size_t value) {
    *((size_t *)buffer) = value;
    *(size) += sizeof(size_t);
    return buffer + sizeof(size_t);
}

static void *readBool(void *buffer, size_t *size, bool *value) {
    *value = *((bool *)buffer);
    *(size) -= sizeof(bool);
    return buffer + sizeof(bool);
}

static void *readUByte(void *buffer, size_t *size, uByte *value) {
    *value = *((uByte *)buffer);
    *(size) -= sizeof(uByte);
    return buffer + sizeof(uByte);
}

static void *readUShort(void *buffer, size_t *size, uShort *value) {
    *value = *((uShort *)buffer);
    *(size) -= sizeof(uShort);
    return buffer + sizeof(uShort);
}

static void *readSInt(void *buffer, size_t *size, sInt *value) {
    *value = *((sInt *)buffer);
    *(size) -= sizeof(sInt);
    return buffer + sizeof(sInt);
}

static void *readSizeT(void *buffer, size_t *size, size_t *value) {
    *value = *((size_t *)buffer);
    *(size) -= sizeof(size_t);
    return buffer + sizeof(size_t);
}

void processPacket(void *packet, size_t size) {
    // Differenciate the packets and process them
    switch (packetGetID(packet)) {
    case PACKET_START: {
        void *buffer = packetGetDataStart(packet);
        size = packetGetDataSize(size);

        // find player index based on network node id
        // and set player uuid in synchronizer
        sInt seed;
        sInt playerCount = 1;
        int playerIndex = 0;

        buffer = readSInt(buffer, &size, &seed);
        buffer = readSInt(buffer, &size, &playerCount);
        for (int i = 0; i < playerCount; i++) {
            uShort nodeID;

            buffer = readUShort(buffer, &size, &nodeID);

            if (nodeID == networkGetLocalNodeID()) {
                playerIndex = i;
            }
        }

        // cleanup transfer tmp file
        FILE *file = fopen("tmpTransfer.bin", "wb");
        if (file != NULL) {
            fclose(file);
            remove("tmpTransfer.bin");
        }

        // init synchronizer
        synchronizerInit(seed, playerCount, playerIndex);

        break;
    }
    case PACKET_START_FILEHEADER: {
        void *data = packetGetDataStart(packet);

        uByte type;
        uByte id;
        size_t fsize;

        data = readUByte(data, &size, &type);
        data = readUByte(data, &size, &id);
        data = readSizeT(data, &size, &fsize);

        recvFile = fopen("tmpTransfer.bin", "wb");
        recvFileSize = fsize;

        break;
    }
    case PACKET_START_FILEDATA: {
        void *data = packetGetDataStart(packet);
        size_t dsize = packetGetDataSize(size);

        size_t toread = dsize;

        fwrite(data, 1, toread, recvFile);

        recvFileSize -= toread;
        if (recvFileSize <= 0) {
            fclose(recvFile);
            recvFile = NULL;
        }

        break;
    }
    case PACKET_START_REQUEST_IDS: {
        synchronizerSendUID();
        break;
    }
    case PACKET_START_ID: {
        uByte playerID = packetGetSender(packet);
        sInt uid;

        void *data = packetGetDataStart(packet);
        size_t dsize = packetGetDataSize(size);

        data = readSInt(data, &dsize, &uid);

        synchronizerSetPlayerUID(playerID, uid);
        synchronizerSendIfReady();
        break;
    }
    case PACKET_START_READY: {
        synchronizerSetPlayerReady(packetGetSender(packet));
        if (playerLocalID == 0) {
            if (synchronizerAllReady()) {
                sendStartSyncPacket();
                synchronizerStart(); // server needs to call this here, all others do when they recieve the packet
            }
        }
        break;
    }

    case PACKET_TURN_START: {
        synchronizerStart();
        break;
    }
    case PACKET_TURN_INPUT: {
        synchronizerOnInputPacket(packetGetSender(packet), packetGetTurn(packet), packetGetDataStart(packet), packetGetDataSize(size));
        break;
    }
    }
}

uByte packetGetID(void *packet) {
    return *((uByte *)packet);
}

uByte packetGetSender(void *packet) {
    return *((uByte *)packet + sizeof(uByte));
}

sInt packetGetTurn(void *packet) {
    return *((sInt *)(packet + sizeof(uByte) + sizeof(uByte)));
}

void *packetGetDataStart(void *packet) {
    return packet + sizeof(uByte) + sizeof(uByte) + sizeof(sInt);
}

size_t packetGetDataSize(size_t size) {
    return size - sizeof(uByte) - sizeof(uByte) - sizeof(sInt);
}

size_t writeStartPacket(void *buffer, sInt seed) {
    size_t size = 0;
    buffer = writeUByte(buffer, &size, PACKET_START);
    buffer = writeUByte(buffer, &size, 0);
    buffer = writeSInt(buffer, &size, 0);

    buffer = writeSInt(buffer, &size, seed);
    buffer = writeSInt(buffer, &size, (sInt)networkGetNodeCount());
    for (int i = 1; i <= NETWORK_MAXNODES; i++) {
        if (networkIsNodeConnected(i)) {
            buffer = writeUShort(buffer, &size, (uShort)i);
        }
    }

    return size;
}

size_t writeStartRequestPacket(void *buffer) {
    size_t size = 0;
    buffer = writeUByte(buffer, &size, PACKET_START_REQUEST_IDS);
    buffer = writeUByte(buffer, &size, 0);
    buffer = writeSInt(buffer, &size, 0);

    return size;
}

size_t writeInputPacket(void *buffer, Inputs *inputs, uByte playerID, sInt turnNumber) {
    size_t size = 0;
    buffer = writeUByte(buffer, &size, PACKET_TURN_INPUT);
    buffer = writeUByte(buffer, &size, playerID);
    buffer = writeSInt(buffer, &size, turnNumber);

    buffer = writeSInt(buffer, &size, inputs->k_touchX);
    buffer = writeSInt(buffer, &size, inputs->k_touchY);

    buffer = writeBool(buffer, &size, inputs->k_up.down);
    buffer = writeBool(buffer, &size, inputs->k_up.clicked);
    buffer = writeBool(buffer, &size, inputs->k_down.down);
    buffer = writeBool(buffer, &size, inputs->k_down.clicked);
    buffer = writeBool(buffer, &size, inputs->k_left.down);
    buffer = writeBool(buffer, &size, inputs->k_left.clicked);
    buffer = writeBool(buffer, &size, inputs->k_right.down);
    buffer = writeBool(buffer, &size, inputs->k_right.clicked);
    buffer = writeBool(buffer, &size, inputs->k_attack.down);
    buffer = writeBool(buffer, &size, inputs->k_attack.clicked);
    buffer = writeBool(buffer, &size, inputs->k_menu.down);
    buffer = writeBool(buffer, &size, inputs->k_menu.clicked);
    buffer = writeBool(buffer, &size, inputs->k_pause.down);
    buffer = writeBool(buffer, &size, inputs->k_pause.clicked);
    buffer = writeBool(buffer, &size, inputs->k_accept.down);
    buffer = writeBool(buffer, &size, inputs->k_accept.clicked);
    buffer = writeBool(buffer, &size, inputs->k_decline.down);
    buffer = writeBool(buffer, &size, inputs->k_decline.clicked);
    buffer = writeBool(buffer, &size, inputs->k_delete.down);
    buffer = writeBool(buffer, &size, inputs->k_delete.clicked);
    buffer = writeBool(buffer, &size, inputs->k_menuNext.down);
    buffer = writeBool(buffer, &size, inputs->k_menuNext.clicked);
    buffer = writeBool(buffer, &size, inputs->k_menuPrev.down);
    buffer = writeBool(buffer, &size, inputs->k_menuPrev.clicked);

    return size;
}

bool readInputPacketData(void *buffer, size_t size, Inputs *inputs) {
    buffer = readSInt(buffer, &size, &(inputs->k_touchX));
    if (size <= 0)
        return false;
    buffer = readSInt(buffer, &size, &(inputs->k_touchY));
    if (size <= 0)
        return false;

    buffer = readBool(buffer, &size, &(inputs->k_up.down));
    buffer = readBool(buffer, &size, &(inputs->k_up.clicked));
    if (size <= 0)
        return false;
    buffer = readBool(buffer, &size, &(inputs->k_down.down));
    buffer = readBool(buffer, &size, &(inputs->k_down.clicked));
    if (size <= 0)
        return false;
    buffer = readBool(buffer, &size, &(inputs->k_left.down));
    buffer = readBool(buffer, &size, &(inputs->k_left.clicked));
    if (size <= 0)
        return false;
    buffer = readBool(buffer, &size, &(inputs->k_right.down));
    buffer = readBool(buffer, &size, &(inputs->k_right.clicked));
    if (size <= 0)
        return false;
    buffer = readBool(buffer, &size, &(inputs->k_attack.down));
    buffer = readBool(buffer, &size, &(inputs->k_attack.clicked));
    if (size <= 0)
        return false;
    buffer = readBool(buffer, &size, &(inputs->k_menu.down));
    buffer = readBool(buffer, &size, &(inputs->k_menu.clicked));
    if (size <= 0)
        return false;
    buffer = readBool(buffer, &size, &(inputs->k_pause.down));
    buffer = readBool(buffer, &size, &(inputs->k_pause.clicked));
    if (size <= 0)
        return false;
    buffer = readBool(buffer, &size, &(inputs->k_accept.down));
    buffer = readBool(buffer, &size, &(inputs->k_accept.clicked));
    if (size <= 0)
        return false;
    buffer = readBool(buffer, &size, &(inputs->k_decline.down));
    buffer = readBool(buffer, &size, &(inputs->k_decline.clicked));
    if (size <= 0)
        return false;
    buffer = readBool(buffer, &size, &(inputs->k_delete.down));
    buffer = readBool(buffer, &size, &(inputs->k_delete.clicked));
    if (size <= 0)
        return false;
    buffer = readBool(buffer, &size, &(inputs->k_menuNext.down));
    buffer = readBool(buffer, &size, &(inputs->k_menuNext.clicked));
    if (size <= 0)
        return false;
    buffer = readBool(buffer, &size, &(inputs->k_menuPrev.down));
    buffer = readBool(buffer, &size, &(inputs->k_menuPrev.clicked));

    return size == 0;
}

void sendFile(FILE *file, uByte fileType, uByte id) {
    fseek(file, 0, SEEK_END);   // seek to end of file
    size_t fsize = ftell(file); // get current file pointer
    fseek(file, 0, SEEK_SET);   // seek back to beginning of file

    // send file header
    void *buffer = networkWriteBuffer;
    size_t size = 0;
    buffer = writeUByte(buffer, &size, PACKET_START_FILEHEADER);
    buffer = writeUByte(buffer, &size, 0);
    buffer = writeSInt(buffer, &size, 0);
    buffer = writeUByte(buffer, &size, fileType);
    buffer = writeUByte(buffer, &size, id);
    buffer = writeSizeT(buffer, &size, fsize);
    networkSend(networkWriteBuffer, size);

    // send file data
    while (fsize > 0) {
        buffer = networkWriteBuffer;
        size = 0;
        buffer = writeUByte(buffer, &size, PACKET_START_FILEDATA);
        buffer = writeUByte(buffer, &size, 0);
        buffer = writeSInt(buffer, &size, 0);

        // read file data
        size_t towrite = NETWORK_MAXDATASIZE - size;
        if (towrite > fsize)
            towrite = fsize;

        fread(buffer, 1, towrite, file);

        size += towrite;
        fsize -= towrite;

        // send file data
        networkSend(networkWriteBuffer, size);
    }
}

void sendIDPacket(uByte playerID, sInt uid) {
    void *buffer = networkWriteBuffer;
    size_t size = 0;

    buffer = writeUByte(buffer, &size, PACKET_START_ID);
    buffer = writeUByte(buffer, &size, playerID);
    buffer = writeSInt(buffer, &size, 0);

    buffer = writeSInt(buffer, &size, uid);

    networkSend(networkWriteBuffer, size);
}

void sendStartReadyPacket(uByte playerID) {
    void *buffer = networkWriteBuffer;
    size_t size = 0;

    buffer = writeUByte(buffer, &size, PACKET_START_READY);
    buffer = writeUByte(buffer, &size, playerID);
    buffer = writeSInt(buffer, &size, 0);

    networkSend(networkWriteBuffer, size);
}

void sendStartSyncPacket() {
    void *buffer = networkWriteBuffer;
    size_t size = 0;

    buffer = writeUByte(buffer, &size, PACKET_TURN_START);
    buffer = writeUByte(buffer, &size, 0);
    buffer = writeSInt(buffer, &size, 0);

    networkSend(networkWriteBuffer, size);
}
