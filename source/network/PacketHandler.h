#pragma once

#include "../Player.h"
#include <stdio.h>
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

extern uByte packetGetID(void *packet);
extern uByte packetGetSender(void *packet);
extern sInt packetGetTurn(void *packet);
extern void *packetGetDataStart(void *packet);
extern size_t packetGetDataSize(size_t size);

extern size_t writeStartPacket(void *buffer, sInt seed);
extern size_t writeStartRequestPacket(void *buffer);

extern size_t writeInputPacket(void *buffer, Inputs *inputs, uByte playerID, sInt turnNumber);
extern bool readInputPacketData(void *buffer, size_t size, Inputs *inputs);

extern void sendFile(FILE *file, uByte fileType, uByte id);
extern void sendIDPacket(uByte playerID, sInt uid);
extern void sendStartReadyPacket(uByte playerID);
extern void sendStartSyncPacket();
