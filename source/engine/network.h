#pragma once
#include "dtypes.h"
#include <stdbool.h>
#include <stddef.h>

#define NETWORK_WLANCOMMID 0x11441850
#define NETWORK_PASSPHRASE "minicraft3dsLP"
#define NETWORK_CHANNEL 1

#define NETWORK_MAXPLAYERS 8
#define NETWORK_MAXNODES 16
#define NETWORK_MAXDATASIZE 1024

typedef void (*PacketHandler)(void *packet, size_t size);

extern void *networkWriteBuffer;

extern void initNetwork(PacketHandler ph);
extern void exitNetwork();

extern bool networkAvailable();

extern bool networkHost();
extern void networkHostStopConnections();
extern bool networkScan();
extern int networkGetScanCount();
extern bool networkGetScanName(char *name, int pos);
extern bool networkConnect(int pos);
extern void networkDisconnect();

extern void networkStart();

extern void networkUpdateStatus();
extern bool networkConnected();

extern int networkGetNodeCount();
extern int networkGetLocalNodeID();
extern bool networkIsNodeConnected(int id);
extern bool networkGetNodeName(int id, char *name);

extern int networkGetExpectedSeqFrom(int id);
extern bool networkSeqIsLowerThan(int firstID, int secondID);

extern void networkSend(void *packet, size_t size);
extern void networkSendWaitFlush();
