#pragma once
#include "dtypes.h"
#include <stddef.h>

#define NETWORK_WLANCOMMID 0x11441850
#define NETWORK_PASSPHRASE "minicraft3dsLP"
#define NETWORK_CHANNEL 1

#define NETWORK_MAXPLAYERS 8
#define NETWORK_MAXNODES 16
#define NETWORK_MAXDATASIZE 1024

typedef void (*PacketHandler)(void *packet, uShort size);

extern void initNetwork(PacketHandler ph);
extern void exitNetwork();

extern bool networkAvailable();

extern bool networkHost();
extern void networkHostStopConnections();
extern uShort networkScan();
extern bool networkGetScanName(char *name, int pos);
extern bool networkConnect(int pos);
extern void networkDisconnect();

extern bool networkConnected();

extern int networkGetNodeCount();
extern bool networkGetNodeName(int id, char *name);

extern void networkSend(void *packet, uShort size);
