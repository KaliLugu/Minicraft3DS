#pragma once
#include "../engine/engine.h"

typedef void (*MessageHandler)(sInt senderID, uByte senderPlayerIndex, void *message, uShort size);
typedef void (*Callback)(void);

// TODO: How to start? - maybe:
//  Server/host sends request start and collects other players ids via the acks (server should hopefully know number of connected players)
//  Server assigns player numbers
//  Server sends packet with player number -> id mapping (simple list)

extern void rp2pInit(bool isHost, sInt localID, MessageHandler mh, Callback onStart);
extern void rp2pExit();
extern bool rp2pIsActive();

extern void rp2pRecvRaw(void *packet, uShort size);

extern void rp2pSend(void *message, uShort size);
extern void rp2pFlush();

extern uByte rp2pGetPlayerCount();
extern uByte rp2pGetLocalPlayerIndex();
extern uByte rp2pGetPlayerID(uByte playerIndex);
