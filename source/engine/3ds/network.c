#ifdef __3DS__
#include "../network.h"

#include <3ds.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>

#define NETWORK_RECVBUFSIZE UDS_DEFAULT_RECVBUFSIZE

#define NETWORK_SENDBUFFERSIZE ((NETWORK_MAXDATASIZE + 256) * 10)

#define NETWORK_STACKSIZE (8 * 1024)

void *networkWriteBuffer;

static bool udsRunning;
static PacketHandler packetHandler;

static size_t scannedNetworksCount;
static udsNetworkScanInfo *scannedNetworks;

static bool isConnected;
static bool isServer;

static size_t networkBufferSize;
static void *networkBuffer;

static udsNetworkStruct networkStruct;
static udsBindContext networkBindCtx;

static udsConnectionStatus networkStatus;

static uShort networkConnectedMask;

// new code
// structure in buffer is uShort(seqID),uShort(size),size(data), ...
static void *networkSendBuffer;
static size_t networkSendBufferStartPos;
static size_t networkSendBufferEndPos;
static size_t networkSendBufferWrapPos;

static uShort networkSeqSendNext;
static uShort networkSeqSendConf[UDS_MAXNODES + 1];
static uShort networkSeqRecvLast[UDS_MAXNODES + 1];
static void *networkAckBuffer;

// async internal send/recieve handling
static Thread networkThread;
static volatile bool networkRunThread;
static LightLock sendBufferLock;

static void networkHandleSend();
static void networkHandleRecieve();
static void clearSendAckedBuffer();
static bool sendAck(uShort target, uShort ack);

static void networkThreadMain(void *arg) {
    while (networkRunThread) {
        if (udsRunning && isConnected) {
            networkUpdateStatus();
            networkHandleRecieve();
            networkHandleSend();
        }

        // TODO: Set meaningfull value, WARNING: Setting this near 1ms (1000*1000) will make everything super laggy, higher values actually work better!
        svcSleepThread(10000 * 1000);
    }
}

void networkUpdateStatus() {
    /*for(int i=0; i<10; i++) {
        Result ret = udsGetConnectionStatus(&networkStatus);
        if(!R_FAILED(ret)) {
            return;
        }
    }*/
    if (udsWaitConnectionStatusEvent(false, false)) {
        udsGetConnectionStatus(&networkStatus);
    }
}

static void networkHandleRecieve() {
    bool recieved = false;
    do {
        recieved = false;

        size_t actualSize = 0;
        uShort sourceNetworkNodeID;
        sInt ackToSend = 0;

        memset(networkBuffer, 0, networkBufferSize);

        Result ret = udsPullPacket(&networkBindCtx, networkBuffer, networkBufferSize, &actualSize, &sourceNetworkNodeID);
        if (R_FAILED(ret)) {
            // TODO: what do?

            // actualSize will be 0 if no packet is available
        } else if (actualSize) {
            void *readPointer = networkBuffer;

            // ack frame
            if (actualSize == sizeof(uShort)) {
                networkSeqSendConf[sourceNetworkNodeID] = *((uShort *)readPointer);
                clearSendAckedBuffer();
                // normal frame
            } else {
                while (actualSize > 0) {
                    // read seqID and size
                    uShort seqID = *((uShort *)readPointer);
                    readPointer += sizeof(uShort);
                    actualSize -= sizeof(uShort);

                    uShort size = *((uShort *)readPointer);
                    readPointer += sizeof(uShort);
                    actualSize -= sizeof(uShort);

                    // if the seq id was expected handle the packet
                    uShort nextID = networkGetExpectedSeqFrom(sourceNetworkNodeID);
                    if (seqID == nextID) {
                        networkSeqRecvLast[sourceNetworkNodeID] = seqID;
                        ackToSend = seqID;

                        // handle data - TODO: WARNING: Do not send sizeof(uShort) packets or else they will get confused with this one
                        if (size == sizeof(uShort)) {
                            networkConnectedMask = *((uShort *)readPointer);
                        } else {
                            packetHandler(readPointer, size);
                        }
                    } else if (networkSeqIsLowerThan(seqID, nextID)) {
                        ackToSend = seqID;
                    }
                    readPointer += size;
                    actualSize -= size;
                }

                if (ackToSend != 0) {
                    if (sendAck(sourceNetworkNodeID, ackToSend)) {
                    }
                }
            }

            recieved = true;
        }
    } while (recieved);
}

static void networkHandleSend() {
    if (networkSendBufferStartPos != networkSendBufferEndPos) {
        LightLock_Lock(&sendBufferLock);

        // determine send size
        size_t currentSize = 0;
        while (networkSendBufferStartPos + currentSize < networkSendBufferWrapPos && networkSendBufferStartPos + currentSize != networkSendBufferEndPos) {
            // size of "header info" (seqid,size)
            size_t extraSize = sizeof(uShort) + sizeof(uShort);
            // data size
            extraSize += *((uShort *)((networkSendBuffer + networkSendBufferStartPos + currentSize) + sizeof(uShort)));

            // if next packet can fit in frame include it
            if (currentSize + extraSize < UDS_DATAFRAME_MAXSIZE) {
                currentSize += extraSize;
            } else {
                break;
            }
        }

        // send frame
        if (currentSize > 0) {
            // TODO: Once we have our own custom mask, no longer broadcast, but send directly because bcast doesn't always reach everyone?
            if (networkConnectedMask == 0) {
                // send frame
                Result ret = udsSendTo(UDS_BROADCAST_NETWORKNODEID, NETWORK_CHANNEL, UDS_SENDFLAG_Default, networkSendBuffer + networkSendBufferStartPos, currentSize);
                if (UDS_CHECK_SENDTO_FATALERROR(ret)) {
                    // TODO: what do?
                } else if (R_FAILED(ret)) {
                    // TODO: what do?
                }
            } else {
                for (int i = 1; i <= UDS_MAXNODES; i++) {
                    if (i != networkGetLocalNodeID() /* && networkIsNodeConnected(i)*/ && networkConnectedMask & (1 << (i - 1))) {
                        // send frame
                        Result ret = udsSendTo(i, NETWORK_CHANNEL, UDS_SENDFLAG_Default, networkSendBuffer + networkSendBufferStartPos, currentSize);
                        if (UDS_CHECK_SENDTO_FATALERROR(ret)) {
                            // TODO: what do?
                        } else if (R_FAILED(ret)) {
                            // TODO: what do?
                        }
                    }
                }
            }
        }

        LightLock_Unlock(&sendBufferLock);
    }
}

static void clearSendAckedBuffer() {
    // find last ack recieved from all com partners
    uShort ackID = 0;
    for (int i = 1; i <= UDS_MAXNODES; i++) {
        if (i != networkGetLocalNodeID() /* && networkIsNodeConnected(i)*/ && networkConnectedMask & (1 << (i - 1))) {
            if (networkSeqSendConf[i] == 0) {
                ackID = 0;
                return;
            }

            if (ackID == 0) {
                ackID = networkSeqSendConf[i];
            } else if (networkSeqSendConf[i] < 100) {
                if (ackID > networkSeqSendConf[i] && ackID < 65535 - 100)
                    ackID = networkSeqSendConf[i];
            } else if (networkSeqSendConf[i] > 65535 - 100) {
                if (ackID > networkSeqSendConf[i] || ackID < 100)
                    ackID = networkSeqSendConf[i];
            } else {
                if (ackID > networkSeqSendConf[i])
                    ackID = networkSeqSendConf[i];
            }
        }
    }
    if (ackID == 0)
        return;

    LightLock_Lock(&sendBufferLock);

    // clear buffer of acknowledgt packets
    while (networkSendBufferStartPos != networkSendBufferEndPos) {
        // find current seqid and size
        uShort seqID = *((uShort *)(networkSendBuffer + networkSendBufferStartPos));
        uShort size = *((uShort *)(networkSendBuffer + networkSendBufferStartPos + sizeof(uShort)));

        if (seqID <= ackID || (ackID < 100 && seqID > 65535 - 100)) {

            size_t currentSize = sizeof(uShort) * 2 + size;

            // adjust buffer "pointers"
            networkSendBufferStartPos += currentSize;
            if (networkSendBufferStartPos == networkSendBufferEndPos) {
                networkSendBufferStartPos = 0;
                networkSendBufferEndPos = 0;
                networkSendBufferWrapPos = 0;
            }

            // wrap
            if (networkSendBufferStartPos == networkSendBufferWrapPos) {
                networkSendBufferStartPos = 0;
                networkSendBufferWrapPos = networkSendBufferEndPos;
            }
        } else {
            break;
        }
    }
    LightLock_Unlock(&sendBufferLock);
}

static bool sendAck(uShort target, uShort ack) {
    Result ret = udsSendTo(target, NETWORK_CHANNEL, UDS_SENDFLAG_Default, &ack, sizeof(uShort));
    if (UDS_CHECK_SENDTO_FATALERROR(ret)) {
        // TODO: what do?
        return false;
    } else if (R_FAILED(ret)) {
        // TODO: what do?
        return false;
    } else {
        return true;
    }
}

void initNetwork(PacketHandler ph) {
    packetHandler = ph;

    Result ret = udsInit(0x3000, NULL);
    if (R_FAILED(ret)) {
        udsRunning = false;
    } else {
        udsRunning = true;

        scannedNetworksCount = 0;
        scannedNetworks = NULL;
        isConnected = false;
        isServer = false;
        networkConnectedMask = 0;

        networkWriteBuffer = malloc(NETWORK_MAXDATASIZE);
        if (networkWriteBuffer == NULL) {
            exitNetwork();
            return;
        }

        networkBufferSize = 0x4000;
        networkBuffer = malloc(networkBufferSize);
        if (networkBuffer == NULL) {
            exitNetwork();
            return;
        }

        networkSendBufferStartPos = 0;
        networkSendBufferEndPos = 0;
        networkSendBufferWrapPos = 0;
        networkSendBuffer = malloc(NETWORK_SENDBUFFERSIZE);
        if (networkSendBuffer == NULL) {
            exitNetwork();
            return;
        }

        networkSeqSendNext = 1;
        for (int i = 0; i < UDS_MAXNODES + 1; i++) {
            networkSeqSendConf[i] = 0;
            networkSeqRecvLast[i] = 0;
        }

        networkAckBuffer = malloc(sizeof(uShort) + sizeof(uShort) + sizeof(uShort));
        if (networkAckBuffer == NULL) {
            exitNetwork();
            return;
        }

        LightLock_Init(&sendBufferLock);

        sInt prio = 0;
        svcGetThreadPriority(&prio, CUR_THREAD_HANDLE);

        // NOTE: It is important the networkThread is prioritized over the main thread (so substract 2) or else nothing will work
        networkRunThread = true;
        networkThread = threadCreate(networkThreadMain, NULL, NETWORK_STACKSIZE, prio - 1, -2, false);
    }
}

void exitNetwork() {
    // Additionally to shutting down the service, clear any left over memory!
    if (udsRunning) {
        udsRunning = false;

        if (networkRunThread) {
            networkRunThread = false;
            threadJoin(networkThread, U64_MAX);
            threadFree(networkThread);
        }

        // cleanup any dynamically reserved memory
        if (scannedNetworks != NULL)
            free(scannedNetworks);
        scannedNetworks = NULL;

        if (networkWriteBuffer != NULL)
            free(networkWriteBuffer);
        networkWriteBuffer = NULL;

        if (networkBuffer != NULL)
            free(networkBuffer);
        networkBuffer = NULL;

        if (networkSendBuffer != NULL)
            free(networkSendBuffer);
        networkSendBuffer = NULL;

        if (networkAckBuffer != NULL)
            free(networkAckBuffer);
        networkAckBuffer = NULL;

        networkDisconnect();

        udsExit();
    }
}

bool networkAvailable() {
    return udsRunning;
}

bool networkHost() {
    if (udsRunning && !isConnected) {
        udsGenerateDefaultNetworkStruct(&networkStruct, NETWORK_WLANCOMMID, 0, NETWORK_MAXPLAYERS);

        Result ret = udsCreateNetwork(&networkStruct, NETWORK_PASSPHRASE, strlen(NETWORK_PASSPHRASE) + 1, &networkBindCtx, NETWORK_CHANNEL, NETWORK_RECVBUFSIZE);
        if (R_FAILED(ret)) {
            return false;
        } else {
            if (udsWaitConnectionStatusEvent(false, false)) {
            }
            udsGetConnectionStatus(&networkStatus);
            udsSetNewConnectionsBlocked(false, true, false);
            isConnected = true;
            isServer = true;
            networkConnectedMask = 0;
            return true;
        }
    }
    return false;
}

void networkHostStopConnections() {
    udsSetNewConnectionsBlocked(true, true, false);
}

bool networkScan() {
    if (udsRunning) {
        // reset values from last scan
        if (scannedNetworks != NULL)
            free(scannedNetworks);
        scannedNetworks = NULL;
        scannedNetworksCount = 0;

        // scan
        memset(networkBuffer, 0, networkBufferSize);
        Result ret = udsScanBeacons(networkBuffer, networkBufferSize, &scannedNetworks, &scannedNetworksCount, NETWORK_WLANCOMMID, 0, NULL, isConnected);
        if (R_FAILED(ret)) {
            // TODO: what do?
            scannedNetworksCount = 0;
            return false;
        }

        return true;
    }
    return false;
}

int networkGetScanCount() {
    if (udsRunning) {
        return scannedNetworksCount;
    } else {
        return 0;
    }
}

bool networkGetScanName(char *name, int pos) {
    if (udsRunning) {
        if (pos < 0 || pos >= scannedNetworksCount)
            return false;

        Result ret = udsGetNodeInfoUsername(&(scannedNetworks[pos].nodes[0]), name);
        if (R_FAILED(ret)) {
            // TODO: what do?
            return false;
        }
        return true;
    }
    return false;
}

bool networkConnect(int pos) {
    if (udsRunning && !isConnected) {
        if (pos < 0 || pos >= scannedNetworksCount)
            return false;

        Result ret = udsConnectNetwork(&scannedNetworks[pos].network, NETWORK_PASSPHRASE, strlen(NETWORK_PASSPHRASE) + 1, &networkBindCtx, UDS_BROADCAST_NETWORKNODEID, UDSCONTYPE_Client, NETWORK_CHANNEL, NETWORK_RECVBUFSIZE);
        if (R_FAILED(ret)) {
            return false;
        } else {
            if (udsWaitConnectionStatusEvent(false, false)) {
            }
            udsGetConnectionStatus(&networkStatus);
            isConnected = true;
            isServer = false;
            networkConnectedMask = 0;
            return true;
        }
    }
    return false;
}

void networkDisconnect() {
    // For clients this just means disconnect, for the server it means destroy the network
    if (udsRunning && isConnected) {
        isConnected = false;

        LightLock_Lock(&sendBufferLock);
        // reset send buffer
        networkSendBufferStartPos = 0;
        networkSendBufferEndPos = 0;
        networkSendBufferWrapPos = 0;

        // reset ack status
        networkSeqSendNext = 1;
        for (int i = 0; i < UDS_MAXNODES + 1; i++) {
            networkSeqSendConf[i] = 0;
            networkSeqRecvLast[i] = 0;
        }
        LightLock_Unlock(&sendBufferLock);

        // With new changes citra now crashes HOST with "cannot be a router if we are not a host" when exiting game with more than 2 people
        svcSleepThread(220000 * 1000); // HACK: prevent citra crash (>20*networkthreadsleep) (wait unti no more stuff gets send)

        // TODO
        if (isServer) {
            // TODO: Clients need to cleanup too, how can I tell they got disconnected
            udsDestroyNetwork();
        } else {
            udsDisconnectNetwork();
        }
        udsUnbind(&networkBindCtx);

        isServer = false;
    }
}

void networkStart() {
    // TODO: This sends the node_bitmask from server to everyone else, because it is uncorrect on some clients?
    if (udsRunning && isConnected && isServer) {
        void *buffer = networkWriteBuffer;

        *((uShort *)buffer) = networkStatus.node_bitmask;
        networkConnectedMask = networkStatus.node_bitmask;

        networkSend(networkWriteBuffer, sizeof(uShort));
        networkSendWaitFlush();
    }
}

bool networkConnected() {
    return isConnected;
}

int networkGetNodeCount() {
    if (udsRunning && isConnected) {
        return networkStatus.total_nodes;
    } else {
        return 0;
    }
}

int networkGetLocalNodeID() {
    if (udsRunning && isConnected) {
        return networkStatus.cur_NetworkNodeID;
    } else {
        return 0;
    }
}

bool networkIsNodeConnected(int id) {
    if (udsRunning && isConnected) {
        return networkStatus.node_bitmask & (1 << (id - 1));
    } else {
        return false;
    }
}

bool networkGetNodeName(int id, char *name) {
    if (udsRunning && isConnected && networkIsNodeConnected(id)) {
        udsNodeInfo nodeInfo;
        udsGetNodeInformation(id, &nodeInfo);

        Result ret = udsGetNodeInfoUsername(&nodeInfo, name);
        if (R_FAILED(ret)) {
            // TODO: what do?
            return false;
        }
        return true;
    }
    return false;
}

int networkGetExpectedSeqFrom(int id) {
    uShort nextID = networkSeqRecvLast[id];
    nextID += 1;
    if (nextID == 0) {
        nextID = 1;
    }
    return nextID;
}

bool networkSeqIsLowerThan(int firstID, int secondID) {
    if (secondID < 100) {
        return (firstID < secondID) || (firstID > 65536 - 100);
    } else if (secondID > 65536 - 100) {
        return (firstID < secondID) && (firstID > 100);
    } else {
        return (firstID < secondID);
    }
}

static int fitInSendBuffer(size_t size) {
    // add "header" length
    size += sizeof(uShort) * 2;

    // we have no wrap currently
    if (networkSendBufferStartPos <= networkSendBufferEndPos) {
        // and can fit without wrap
        if (networkSendBufferEndPos + size < NETWORK_SENDBUFFERSIZE) {
            networkSendBufferEndPos += size;
            networkSendBufferWrapPos = networkSendBufferEndPos;

            return networkSendBufferEndPos - size;
            // we need to wrap
        } else {
            if (size < networkSendBufferStartPos) {
                networkSendBufferEndPos = size;

                return 0;
            }
        }
        // we have wrap currently
    } else {
        int available = networkSendBufferStartPos - networkSendBufferEndPos - 1;
        if (available > size) {
            networkSendBufferEndPos += size;

            return networkSendBufferEndPos - size;
        }
    }

    return -1;
}

void networkSend(void *packet, size_t size) {
    // search for fit in buffer (and BLOCK until free space is found)
    LightLock_Lock(&sendBufferLock);
    int pos = fitInSendBuffer(size);
    while (pos == -1) {
        LightLock_Unlock(&sendBufferLock);
        svcSleepThread(4500 * 1000); // TODO: Set meaningfull value
        LightLock_Lock(&sendBufferLock);

        pos = fitInSendBuffer(size);
    }

    // fit found -> space is allready "reserved" -> write packet to buffer
    void *writePointer = networkSendBuffer + pos;

    // write seq number
    *((uShort *)writePointer) = networkSeqSendNext;
    networkSeqSendNext++;
    if (networkSeqSendNext == 0) {
        networkSeqSendNext = 1;
    }
    writePointer += sizeof(uShort);

    // write size
    *((uShort *)writePointer) = (uShort)size;
    writePointer += sizeof(uShort);

    // write data
    memcpy(writePointer, packet, size);
    writePointer += size;

    LightLock_Unlock(&sendBufferLock);
}

void networkSendWaitFlush() {
    while (networkSendBufferStartPos != networkSendBufferEndPos) {
        svcSleepThread(4500 * 1000);
    }
}
#endif
