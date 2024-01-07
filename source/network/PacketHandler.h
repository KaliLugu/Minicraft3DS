#pragma once

#include "../Player.h"
#include "Network.h"
#include <stdlib.h>

#define PACKET_START 0
#define PACKET_START_FILEHEADER 1
#define PACKET_START_FILEDATA 2
#define PACKET_START_REQUEST_IDS 3
#define PACKET_START_ID 4
#define PACKET_START_READY 5

#define PACKET_TURN_START 10
#define PACKET_TURN_INPUT 11

extern void processPacket(void *packet, size_t size);

extern u8 packetGetID(void *packet);
extern u8 packetGetSender(void *packet);
extern u32 packetGetTurn(void *packet);
extern void *packetGetDataStart(void *packet);
extern size_t packetGetDataSize(size_t size);

extern size_t writeStartPacket(void *buffer, u32 seed);
extern size_t writeStartRequestPacket(void *buffer);

extern size_t writeInputPacket(void *buffer, Inputs *inputs, u8 playerID, u32 turnNumber);
extern bool readInputPacketData(void *buffer, size_t size, Inputs *inputs);

extern void sendFile(FILE *file, u8 fileType, u8 id);
extern void sendIDPacket(u8 playerID, u32 uid);
extern void sendStartReadyPacket(u8 playerID);
extern void sendStartSyncPacket();
