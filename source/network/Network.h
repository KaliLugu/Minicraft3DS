#pragma once

#include <3ds.h>

#define NETWORK_WLANCOMMID 0x11441850
#define NETWORK_PASSPHRASE "minicraft3dsLP"
#define NETWORK_CHANNEL 1

#define NETWORK_RECVBUFSIZE UDS_DEFAULT_RECVBUFSIZE

#define NETWORK_MAXDATASIZE 1024
#define NETWORK_SENDBUFFERSIZE ((NETWORK_MAXDATASIZE + 256) * 10)

#define NETWORK_STACKSIZE (8 * 1024)

#define NETWORK_MAXPLAYERS 8

extern void *networkWriteBuffer;

extern void networkInit();
extern void networkExit();

extern bool networkAvailable();

extern bool networkHost();
extern void networkHostStopConnections();
extern void networkScan();
extern int networkGetScanCount();
extern bool networkGetScanName(char *name, int pos);
extern bool networkConnect(int pos);
extern void networkDisconnect();

extern void networkStart();

extern void networkUpdateStatus();
extern bool networkConnected();

extern int networkGetNodeCount();
extern u16 networkGetLocalNodeID();
extern bool networkIsNodeConnected(u16 id);
extern bool networkGetNodeName(u16 id, char *name);

extern u16 networkGetExpectedSeqFrom(u16 id);
extern bool networkSeqIsLowerThan(u16 firstID, u16 secondID);

extern void networkSend(void *packet, size_t size);
extern void networkSendWaitFlush();
