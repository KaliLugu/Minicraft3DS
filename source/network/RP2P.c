#include "RP2P.h"
#include <string.h>

#define SENDBUFFERSIZE ((NETWORK_MAXDATASIZE + 256) * 10)
#define RECIEVEBUFFERSIZE ((NETWORK_MAXDATASIZE + 256) * 100)
#define HEADER_SIZE (sizeof(uShort) + sizeof(uByte) + sizeof(sInt))
#define ACK_SIZE (sizeof(uShort) + sizeof(uByte) + sizeof(sInt) * 2)
#define INVALID_PLAYER 255
#define STACKSIZE (8 * 1024)

#define RP2P_MESSAGE_TYPE_INTERNAL_ACK 0
#define RP2P_MESSAGE_TYPE_INTERNAL_PING 1
#define RP2P_MESSAGE_TYPE_INTERNAL_START 2
#define RP2P_MESSAGE_TYPE_APP 64

enum RP2PState {
    rp2pUninitialzed = 0,
    rp2pInitHost,
    rp2pInitClient,
    rp2pBeforeStart,
    rp2pActive
};

struct _rp2pData {
    enum RP2PState state;
    uByte localPlayerIndex;
    sInt localID;
    sInt players[NETWORK_MAXPLAYERS];
    uByte playerCount;

    Callback onStart;
    MessageHandler mh;

    MThread sendThread;
};
static struct _rp2pData rp2pData = {0};

static uShort sendNextSeq;
static uShort seqSendConfirmed[NETWORK_MAXPLAYERS] = {0};
static uShort seqLastRecieved[NETWORK_MAXPLAYERS] = {0};

static void *sendBuffer;
static size_t sendBufferStartPos;
static size_t sendBufferEndPos;
static size_t sendBufferWrapPos;
static Lock sendBufferLock;

static void *recieveBuffer;
static size_t recieveBufferPos;
static Lock recieveBufferLock;

static void clearSendAckedBuffer();
static void rp2pHostStartWhenReady();

static uByte getPlayerIndex(sInt id);
static uShort expectedSeqFrom(uByte playerIndex);
static bool isSeqLowerThan(uShort firstSeq, uShort secondSeq);
static int fitInSendBuffer(uShort size);
static void sendAck(sInt sender, uShort ack);
static void rp2pSendInternal(void *packet, uShort size, uByte type);
static void handleSend();

void rp2pInit(bool isHost, sInt localID, Callback onStart, MessageHandler mh) {
    if (rp2pData.state != rp2pUninitialzed)
        return;

    rp2pData.state = isHost ? rp2pInitHost : rp2pInitClient;
    rp2pData.localID = localID;
    rp2pData.onStart = onStart;
    rp2pData.mh = mh;

    sendNextSeq = 1;
    for (int i = 0; i < NETWORK_MAXPLAYERS; i++) {
        rp2pData.players[i] = 0;
        seqSendConfirmed[i] = 0;
        seqLastRecieved[i] = 0;
    }
    if (isHost) {
        rp2pData.players[0] = localID;
        rp2pData.localPlayerIndex = 0;
        rp2pData.playerCount = 1;
    } else {
        rp2pData.localPlayerIndex = INVALID_PLAYER;
        rp2pData.playerCount = 0;
    }

    sendBufferStartPos = 0;
    sendBufferEndPos = 0;
    sendBufferWrapPos = 0;
    sendBuffer = malloc(SENDBUFFERSIZE);
    sendBufferLock = lockCreate();

    recieveBufferPos = 0;
    recieveBuffer = malloc(RECIEVEBUFFERSIZE);
    recieveBufferLock = lockCreate();

    rp2pData.sendThread = mthreadCreate(&handleSend, STACKSIZE);

    // start if host by pinging to find out client ids
    if (isHost) {
        uByte empty[1];
        rp2pSendInternal(&empty, 0, RP2P_MESSAGE_TYPE_INTERNAL_PING);
    }
}

void rp2pTick() {
    if (rp2pData.state == rp2pBeforeStart) {
        rp2pFlush();
        rp2pData.state = rp2pActive;
        rp2pData.onStart();
    } else if (rp2pData.state == rp2pActive && recieveBufferPos > 0) {
        lockLock(recieveBufferLock);
        size_t recieveBufferRead = 0;
        while (recieveBufferRead < recieveBufferPos) {
            sInt sender = *((sInt *)(recieveBuffer + recieveBufferRead));
            recieveBufferRead += sizeof(sInt);
            uByte player = *((uByte *)(recieveBuffer + recieveBufferRead));
            recieveBufferRead += sizeof(uByte);
            uShort size = *((uShort *)(recieveBuffer + recieveBufferRead));
            recieveBufferRead += sizeof(uShort);
            rp2pData.mh(sender, player, (recieveBuffer + recieveBufferRead), size);
            recieveBufferRead += size;
        }
        recieveBufferPos = 0;
        lockUnlock(recieveBufferLock);
    }
}

void rp2pExit() {
    if (rp2pData.state == rp2pUninitialzed)
        return;

    // exit thread
    rp2pData.state = rp2pUninitialzed;
    mthreadJoin(rp2pData.sendThread);

    // cleanup
    free(sendBuffer);
    lockFree(sendBufferLock);
    free(recieveBuffer);
    lockFree(recieveBufferLock);
}

bool rp2pIsActive() {
    return rp2pData.state == rp2pActive;
}

void rp2pRecvRaw(void *packet, uShort size) {
    if (rp2pData.state == rp2pUninitialzed)
        return;

    // read seqID
    uShort seqID = *((uShort *)packet);
    packet += sizeof(uShort);

    // read type
    uByte type = *((uByte *)packet);
    packet += sizeof(uByte);

    // read sender
    sInt sender = *((sInt *)packet);
    packet += sizeof(sInt);

    uByte player = getPlayerIndex(sender);
    if (player != INVALID_PLAYER) {
        if (type == RP2P_MESSAGE_TYPE_INTERNAL_ACK) {
            // ack frame
            sInt ackTo = *((sInt *)packet);
            packet += sizeof(sInt);
            if (ackTo == rp2pData.localID) {
                seqSendConfirmed[player] = seqID;
                if (rp2pData.state == rp2pInitHost) {
                    rp2pHostStartWhenReady();
                } else {
                    clearSendAckedBuffer();
                }
            }
        } else {
            // if the seq id was expected handle the packet
            uShort expectedSeq = expectedSeqFrom(player);
            if (seqID == expectedSeq) {
                seqLastRecieved[player] = seqID;

                if (type == RP2P_MESSAGE_TYPE_INTERNAL_START && rp2pData.state == rp2pInitClient) {
                    rp2pData.playerCount = *((uByte *)packet);
                    packet += sizeof(uByte);
                    for (uByte i = 0; i < rp2pData.playerCount; i++) {
                        rp2pData.players[i] = *((sInt *)packet);
                        packet += sizeof(sInt);

                        if (rp2pData.players[i] == rp2pData.localID) {
                            rp2pData.localPlayerIndex = i;
                        }
                    }
                    rp2pData.state = rp2pBeforeStart;
                } else if (type == RP2P_MESSAGE_TYPE_APP) {
                    lockLock(recieveBufferLock);
                    *((sInt *)(recieveBuffer + recieveBufferPos)) = sender;
                    recieveBufferPos += sizeof(sInt);
                    *((uByte *)(recieveBuffer + recieveBufferPos)) = player;
                    recieveBufferPos += sizeof(uByte);
                    *((uShort *)(recieveBuffer + recieveBufferPos)) = size - HEADER_SIZE;
                    recieveBufferPos += sizeof(uShort);
                    memcpy(recieveBuffer + recieveBufferPos, packet, size - HEADER_SIZE);
                    recieveBufferPos += size - HEADER_SIZE;
                    lockUnlock(recieveBufferLock);
                }
            }

            if (seqLastRecieved[player] != 0)
                sendAck(sender, seqLastRecieved[player]);
        }
    }
}

void rp2pSend(void *packet, uShort size) {
    if (rp2pData.state != rp2pActive)
        return;

    rp2pSendInternal(packet, size, RP2P_MESSAGE_TYPE_APP);
}

void rp2pFlush() {
    while (sendBufferStartPos != sendBufferEndPos) {
        mthreadSleep(10 * 1000 * 1000);
    }
}

uByte rp2pGetPlayerCount() {
    return rp2pData.playerCount;
}

uByte rp2pGetLocalPlayerIndex() {
    return rp2pData.localPlayerIndex;
}

sInt rp2pGetPlayerID(uByte playerIndex) {
    if (playerIndex >= rp2pData.playerCount)
        return 0;
    return rp2pData.players[playerIndex];
}

static uByte getPlayerIndex(sInt id) {
    // find existing entry
    uByte index;
    for (index = 0; index < NETWORK_MAXPLAYERS; index++) {
        if (rp2pData.players[index] == id)
            return index;
        if (rp2pData.players[index] == 0)
            break;
    }

    if (rp2pData.state == rp2pInitHost && index < NETWORK_MAXPLAYERS) {
        // if in host init -> register client
        rp2pData.players[index] = id;
        rp2pData.playerCount = index + 1;
        return index;
        // if in client init and no registered player yet -> register host
    } else if (rp2pData.state == rp2pInitClient && rp2pData.playerCount <= 0) {
        rp2pData.players[0] = id;
        rp2pData.playerCount = 1;
        return 0;
    } else {
        // else invalid
        return INVALID_PLAYER;
    }
}

static uShort expectedSeqFrom(uByte playerIndex) {
    uShort nextSeq = seqLastRecieved[playerIndex];
    nextSeq += 1;
    if (nextSeq == 0) {
        nextSeq = 1;
    }
    return nextSeq;
}

static bool isSeqLowerThan(uShort firstSeq, uShort secondSeq) {
    if (secondSeq < 100) {
        return (firstSeq < secondSeq) || (firstSeq > 65536 - 100);
    } else if (secondSeq > 65536 - 100) {
        return (firstSeq < secondSeq) && (firstSeq > 100);
    } else {
        return (firstSeq < secondSeq);
    }
}

static int fitInSendBuffer(uShort size) {
    // we have no wrap currently
    if (sendBufferStartPos <= sendBufferEndPos) {
        // and can fit without wrap
        if (sendBufferEndPos + size < SENDBUFFERSIZE) {
            sendBufferEndPos += size;
            sendBufferWrapPos = sendBufferEndPos;

            return sendBufferEndPos - size;
            // we need to wrap
        } else {
            if (size < sendBufferStartPos) {
                sendBufferEndPos = size;

                return 0;
            }
        }
        // we have wrap currently
    } else {
        int available = sendBufferStartPos - sendBufferEndPos - 1;
        if (available > size) {
            sendBufferEndPos += size;

            return sendBufferEndPos - size;
        }
    }

    return -1;
}

static void handleSend() {
    while (true) {
        if (rp2pData.state == rp2pUninitialzed)
            return;

        // send messages
        if (sendBufferStartPos != sendBufferEndPos) {
            lockLock(sendBufferLock);

            uShort size = *((uShort *)(sendBuffer + sendBufferStartPos));
            networkSend(sendBuffer + sendBufferStartPos + sizeof(uShort), size);

            lockUnlock(sendBufferLock);
        }

        mthreadSleep(10 * 1000 * 1000);
    }
}

static void sendAck(sInt sender, uShort ack) {
    uByte sendAckBuffer[ACK_SIZE + 1];
    *((uShort *)sendAckBuffer) = ack;
    *((uByte *)(sendAckBuffer + sizeof(uShort))) = RP2P_MESSAGE_TYPE_INTERNAL_ACK;
    *((sInt *)(sendAckBuffer + sizeof(uShort) + sizeof(uByte))) = rp2pData.localID;
    *((sInt *)(sendAckBuffer + sizeof(uShort) + sizeof(uByte) + sizeof(sInt))) = sender;

    networkSend(sendAckBuffer, ACK_SIZE);
}

static void rp2pSendInternal(void *packet, uShort size, uByte type) {
    // search for fit in buffer (and BLOCK until free space is found)
    lockLock(sendBufferLock);
    int pos = fitInSendBuffer(size + sizeof(uShort) /*size*/ + HEADER_SIZE);
    while (pos == -1) {
        lockUnlock(sendBufferLock);
        mthreadSleep(4 * 1000 * 1000); // TODO: Set meaningfull value
        lockLock(sendBufferLock);

        pos = fitInSendBuffer(size + sizeof(uShort) /*size*/ + HEADER_SIZE);
    }

    // fit found -> space is allready "reserved" -> write packet to buffer
    void *writePointer = sendBuffer + pos;

    // write size
    *((uShort *)writePointer) = (uShort)size + HEADER_SIZE;
    writePointer += sizeof(uShort);

    // write seq number
    *((uShort *)writePointer) = sendNextSeq;
    sendNextSeq++;
    if (sendNextSeq == 0) {
        sendNextSeq = 1;
    }
    writePointer += sizeof(uShort);

    // write type
    *((uByte *)writePointer) = type;
    writePointer += sizeof(uByte);

    // write sender
    *((sInt *)writePointer) = rp2pData.localID;
    writePointer += sizeof(sInt);

    // write data
    memcpy(writePointer, packet, size);
    writePointer += size;

    lockUnlock(sendBufferLock);
}

static void clearSendAckedBuffer() {
    // find last ack recieved from all com partners
    uShort ackByAll = 0;
    for (int i = 0; i < rp2pData.playerCount; i++) {
        if (rp2pData.players[i] != rp2pData.localID) {
            if (seqSendConfirmed[i] == 0) {
                return;
            }

            if (ackByAll == 0) {
                ackByAll = seqSendConfirmed[i];
            } else if (isSeqLowerThan(seqSendConfirmed[i], ackByAll)) {
                ackByAll = seqSendConfirmed[i];
            }
        }
    }
    if (ackByAll == 0)
        return;

    // clear buffer of acknowledgt packets
    lockLock(sendBufferLock);
    while (sendBufferStartPos != sendBufferEndPos) {
        // find current seqid and size
        uShort size = *((uShort *)(sendBuffer + sendBufferStartPos));
        uShort seqID = *((uShort *)(sendBuffer + sendBufferStartPos + sizeof(uShort)));

        if (seqID == ackByAll || isSeqLowerThan(seqID, ackByAll)) {
            uShort skip = sizeof(uShort) + size;

            // adjust buffer "pointers"
            sendBufferStartPos += skip;
            if (sendBufferStartPos == sendBufferEndPos) {
                sendBufferStartPos = 0;
                sendBufferEndPos = 0;
                sendBufferWrapPos = 0;
            }

            // wrap
            if (sendBufferStartPos == sendBufferWrapPos) {
                sendBufferStartPos = 0;
                sendBufferWrapPos = sendBufferEndPos;
            }
        } else {
            break;
        }
    }
    lockUnlock(sendBufferLock);
}

static void rp2pHostStartWhenReady() {
    if (rp2pData.state != rp2pInitHost)
        return;
    if (rp2pData.playerCount != networkGetNodeCount())
        return;
    clearSendAckedBuffer();

    // all players registered -> broadcast playerids
    uShort size = sizeof(uByte) + sizeof(sInt) * rp2pData.playerCount;
    void *packet = malloc(size + 1);
    void *writePointer = packet;
    *((uByte *)writePointer) = rp2pData.playerCount;
    writePointer += sizeof(uByte);
    for (uByte i = 0; i < rp2pData.playerCount; i++) {
        *((sInt *)writePointer) = rp2pData.players[i];
        writePointer += sizeof(sInt);
    }

    rp2pSendInternal(packet, size, RP2P_MESSAGE_TYPE_INTERNAL_START);
    free(packet);
    rp2pData.state = rp2pBeforeStart;
}
