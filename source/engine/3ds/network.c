#ifdef __3DS__
#include "../network.h"

#include <3ds.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>

#define NETWORK_RECVBUFSIZE UDS_DEFAULT_RECVBUFSIZE

#define NETWORK_STACKSIZE (8 * 1024)

static void *networkWriteBuffer;

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

// async internal send/recieve handling
static Thread networkThread;
static volatile bool networkRunThread;

static void networkHandleRecieve() {
    bool recieved = false;
    do {
        recieved = false;

        size_t actualSize = 0;

        memset(networkBuffer, 0, networkBufferSize);

        Result ret = udsPullPacket(&networkBindCtx, networkBuffer, networkBufferSize, &actualSize, NULL);
        if (R_FAILED(ret)) {
            // TODO: what do?

            // actualSize will be 0 if no packet is available
        } else if (actualSize) {
            void *readPointer = networkBuffer;

            while (actualSize > 0) {
                // read size
                uShort size = *((uShort *)readPointer);
                readPointer += sizeof(uShort);
                actualSize -= sizeof(uShort);

                // handle packet
                packetHandler(readPointer, size);
                readPointer += size;
                actualSize -= size;
            }

            recieved = true;
        }
    } while (recieved);
}

static void networkThreadMain(void *arg) {
    while (networkRunThread) {
        if (udsRunning && udsWaitConnectionStatusEvent(false, false)) {
            udsGetConnectionStatus(&networkStatus);
        }
        if (udsRunning && isConnected) {
            networkHandleRecieve();
        }

        // TODO: Set meaningfull value, WARNING: Setting this near 1ms (1000*1000) will make everything super laggy, higher values actually work better!
        svcSleepThread(4 * 1000 * 1000);
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

        // NOTE: It is important the networkThread is prioritized over the main thread (so substract 2) or else nothing will work
        sInt prio = 0;
        svcGetThreadPriority(&prio, CUR_THREAD_HANDLE);
        networkRunThread = true;
        networkThread = threadCreate(networkThreadMain, NULL, NETWORK_STACKSIZE, prio - 1, -2, false);
    }
}

void exitNetwork() {
    // Additionally to shutting down the service, clear any left over memory!
    if (udsRunning) {
        udsRunning = false;

        networkDisconnect();

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
        if (R_SUCCEEDED(ret)) {
            udsWaitConnectionStatusEvent(false, false);
            udsGetConnectionStatus(&networkStatus);
            udsSetNewConnectionsBlocked(false, true, false);
            isConnected = true;
            isServer = true;
            return true;
        }
    }
    return false;
}

void networkHostStopConnections() {
    udsSetNewConnectionsBlocked(true, true, false);
}

uShort networkScan() {
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
            scannedNetworksCount = 0;
            return 0;
        }

        return scannedNetworksCount;
    }
    return 0;
}

bool networkGetScanName(char *name, int pos) {
    if (udsRunning) {
        if (pos < 0 || pos >= scannedNetworksCount)
            return false;

        Result ret = udsGetNodeInfoUsername(&(scannedNetworks[pos].nodes[0]), name);
        return R_SUCCEEDED(ret);
    }
    return false;
}

bool networkConnect(int pos) {
    if (udsRunning && !isConnected) {
        if (pos < 0 || pos >= scannedNetworksCount)
            return false;

        Result ret = udsConnectNetwork(&scannedNetworks[pos].network, NETWORK_PASSPHRASE, strlen(NETWORK_PASSPHRASE) + 1, &networkBindCtx, UDS_BROADCAST_NETWORKNODEID, UDSCONTYPE_Client, NETWORK_CHANNEL, NETWORK_RECVBUFSIZE);
        if (R_SUCCEEDED(ret)) {
            udsWaitConnectionStatusEvent(false, false);
            udsGetConnectionStatus(&networkStatus);
            isConnected = true;
            isServer = false;
            return true;
        }
    }
    return false;
}

void networkDisconnect() {
    // For clients this just means disconnect, for the server it means destroy the network
    if (udsRunning && isConnected) {
        isConnected = false;

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

bool networkGetNodeName(int id, char *name) {
    if (udsRunning && isConnected) {
        int count = 0;
        for (int j = 0; j <= NETWORK_MAXNODES; j++) {
            if (networkStatus.node_bitmask & (1 << j)) {
                if (count == id) {
                    udsNodeInfo nodeInfo;
                    udsGetNodeInformation((1 << j), &nodeInfo);

                    Result ret = udsGetNodeInfoUsername(&nodeInfo, name);
                    return R_SUCCEEDED(ret);
                }
                count++;
            }
        }
    }
    return false;
}

void networkSend(void *packet, uShort size) {
    *((uShort *)networkWriteBuffer) = size;
    memcpy(networkWriteBuffer + sizeof(uShort), packet, size);

    // send frame
    Result ret = udsSendTo(UDS_BROADCAST_NETWORKNODEID, NETWORK_CHANNEL, UDS_SENDFLAG_Default, networkWriteBuffer, sizeof(uShort) + size);
    if (UDS_CHECK_SENDTO_FATALERROR(ret)) {
        // TODO: what do?
    }
}
#endif
