#ifdef __SWITCH__
#include "../engine.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <switch.h>
#include <sys/socket.h>

static bool nwRunning = false;
static PacketHandler packetHandler;

static Lp2pGroupInfo groupInfo = {0};
static Lp2pGroupInfo scanGroupInfo = {0};
static Lp2pScanResult scanResults[16] = {0};
static s32 scanResultsActual = 0;
static Lp2pNodeInfo nodeInfos[NETWORK_MAXNODES] = {0};
static s32 nodeCountActual = 0;

static Lp2pIpConfig ipConfig = {0};
static int socketID = -1;
static struct sockaddr_in serverAddr;
static bool isServer;

static bool createSocket();

// async internal recieve handling
static size_t networkBufferSize;
static void *networkBuffer;

static Thread networkThread;
static volatile bool networkRunThread;

static void networkHandleRecieve() {
    bool recieved = false;
    do {
        recieved = false;

        struct sockaddr_in src_addr = {0};
        socklen_t fromlen = sizeof(struct sockaddr_in);
        ssize_t actualSize = recvfrom(socketID, networkBuffer, networkBufferSize, 0, (struct sockaddr *)&src_addr, &fromlen);

        if (actualSize) {
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
        if (nwRunning && networkConnected()) {
            networkHandleRecieve();
        }

        // TODO: Set meaningfull value, WARNING: Setting this near 1ms (1000*1000) will make everything super laggy, higher values actually work better!
        svcSleepThread(4 * 1000 * 1000);
    }
}

void initNetwork(PacketHandler ph) {
    packetHandler = ph;

    socketID = -1;

    socketInitializeDefault();
    Result ret = lp2pInitialize(Lp2pServiceType_App);
    if (R_FAILED(ret)) {
        socketExit();
        nwRunning = false;
        return;
    } else {
        nwRunning = true;
    }

    networkBufferSize = 0x4000;
    networkBuffer = malloc(networkBufferSize);
    if (networkBuffer == NULL) {
        exitNetwork();
        return;
    }

    networkRunThread = true;
    threadCreate(&networkThread, networkThreadMain, NULL, NULL, 0x10000, 0x2C, -2);
    threadStart(&networkThread);
}

void exitNetwork() {
    if (nwRunning) {
        nwRunning = false;

        if (networkRunThread) {
            networkRunThread = false;
            threadWaitForExit(&networkThread);
            threadClose(&networkThread);
        }

        networkDisconnect();

        lp2pExit();
        socketExit();

        if (networkBuffer != NULL)
            free(networkBuffer);
        networkBuffer = NULL;
    }
}

bool networkAvailable() {
    return nwRunning;
}

bool networkHost() {
    if (nwRunning && !networkConnected()) {
        lp2pCreateGroupInfo(&groupInfo);
        s8 flags = 0;
        lp2pGroupInfoSetFlags(&groupInfo, &flags, 1);
        lp2pGroupInfoSetServiceName(&groupInfo, XSTR(NETWORK_WLANCOMMID));
        lp2pGroupInfoSetPassphrase(&groupInfo, NETWORK_PASSPHRASE);

        Result ret = lp2pCreateGroup(&groupInfo);
        if (R_SUCCEEDED(ret)) {
            isServer = true;
            lp2pGetIpConfig(&ipConfig);
            return createSocket();
        }
    }
    return false;
}

void networkHostStopConnections() {
}

uShort networkScan() {
    if (nwRunning) {
        lp2pCreateGroupInfoScan(&scanGroupInfo);
        s8 flags = 0;
        lp2pGroupInfoSetFlags(&groupInfo, &flags, 1);
        lp2pGroupInfoSetServiceName(&groupInfo, XSTR(NETWORK_WLANCOMMID));
        lp2pGroupInfoSetPassphrase(&groupInfo, NETWORK_PASSPHRASE);

        Result result = lp2pScan(&scanGroupInfo, scanResults, 16, &scanResultsActual);
        if (R_SUCCEEDED(result))
            return scanResultsActual;
    }
    scanResultsActual = 0;
    return 0;
}

bool networkGetScanName(char *name, int pos) {
    if (scanResultsActual <= pos)
        return false;

    // TODO: implement
    name = "";
    return true;
}

bool networkConnect(int pos) {
    if (scanResultsActual <= pos)
        return false;

    Result result = lp2pJoin(&groupInfo, &scanResults[pos].group_info);
    if (R_SUCCEEDED(result)) {
        isServer = false;
        lp2pGetIpConfig(&ipConfig);
        return createSocket();
    }
    return false;
}

void networkDisconnect() {
    if (socketID >= 0) {
        shutdown(socketID, SHUT_RDWR);
        socketID = -1;
    }
    u32 out = 0;
    lp2pLeave(&out);
    lp2pDestroyGroup();
}

bool networkConnected() {
    return socketID >= 0;
}

int networkGetNodeCount() {
    lp2pGetMembers(nodeInfos, NETWORK_MAXNODES, &nodeCountActual);
    return nodeCountActual;
}

bool networkGetNodeName(int id, char *name) {
    if (nodeCountActual <= id)
        return false;
    // TODO:
    name = "";
    return true;
}

void networkSend(void *packet, uShort size) {
    sendto(socketID, packet, size, 0, &serverAddr, sizeof(serverAddr));
}

static bool createSocket() {
    socketID = socket(AF_INET, SOCK_DGRAM, 0);
    if (socketID >= 0) {
        // configure for broadcast
        int optVal = 1;
        int ret = setsockopt(socketID, SOL_SOCKET, SO_BROADCAST, &optVal, sizeof(optVal));
        if (ret == -1) {
            shutdown(socketID, SHUT_RDWR);
            socketID = -1;
            return false;
        } else {
            memset(&serverAddr, 0, sizeof(serverAddr));
            serverAddr.sin_family = AF_INET;
            serverAddr.sin_addr.s_addr = htonl(ntohl(((struct sockaddr_in *)ipConfig.ip_addr)->sin_addr.s_addr) | ~ntohl(((struct sockaddr_in *)ipConfig.subnet_mask)->sin_addr.s_addr));
            serverAddr.sin_port = htons(7777);

            if (bind(socketID, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
                shutdown(socketID, SHUT_RDWR);
                socketID = -1;
                return false;
            }
            return true;
        }
    }
    return false;
}

#endif
