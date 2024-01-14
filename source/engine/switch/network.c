#ifdef __SWITCH__
#include "../engine.h"
#include <stdio.h>
#include <stdlib.h>
#include <switch.h>
#include <sys/socket.h>

static bool nwRunning = false;
static PacketHandler packetHandler;

static Lp2pGroupInfo groupInfo;
static Lp2pGroupInfo scanGroupInfo;

static int socketID = -1;
static struct sockaddr serverAddr;
static bool isServer;

void initNetwork(PacketHandler ph) {
    packetHandler = ph;

    socketInitializeDefault();
    Result ret = lp2pInitialize(Lp2pServiceType_App);
    if (R_FAILED(ret)) {
        socketExit();
        nwRunning = false;
    } else {
        nwRunning = true;
    }
}
void exitNetwork() {
    if (nwRunning) {
        nwRunning = false;

        networkDisconnect();

        lp2pExit();
        socketExit();
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
        if (R_FAILED(ret)) {
            return false;
        } else {
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
                    isServer = true;
                    return true;
                }
            }
        }
    }
    return false;
}

void networkHostStopConnections() {
}

uShort networkScan() {
    return 0;
}

bool networkGetScanName(char *name, int pos) {
    return false;
}

bool networkConnect(int pos) {
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
    return 0;
}

bool networkGetNodeName(int id, char *name) {
    return false;
}

void networkSend(void *packet, size_t size) {
}

#endif
