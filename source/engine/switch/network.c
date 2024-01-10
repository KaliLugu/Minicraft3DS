#ifdef __SWITCH__
#include "../engine.h"

void *networkWriteBuffer;

void initNetwork(PacketHandler ph) {
}
void exitNetwork() {
}

bool networkAvailable() {
    return false;
}

bool networkHost() {
    return false;
}

void networkHostStopConnections() {
}

bool networkScan() {
    return false;
}

int networkGetScanCount() {
    return 0;
}

bool networkGetScanName(char *name, int pos) {
    return false;
}

bool networkConnect(int pos) {
    return false;
}

void networkDisconnect() {
}

void networkStart() {
}

void networkUpdateStatus() {
}

bool networkConnected() {
    return false;
}

int networkGetNodeCount() {
    return 1;
}

int networkGetLocalNodeID() {
    return 0;
}

bool networkIsNodeConnected(int id) {
    return id == 0;
}

bool networkGetNodeName(int id, char *name) {
    return false;
}

int networkGetExpectedSeqFrom(int id) {
    return 0;
}

bool networkSeqIsLowerThan(int firstID, int secondID) {
    return true;
}

void networkSend(void *packet, size_t size) {
}

void networkSendWaitFlush() {
}

#endif
