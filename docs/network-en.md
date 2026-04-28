# Network System — Minicraft 3DS

## Table of Contents

1. [Overview](#overview)
2. [Layered Architecture](#layered-architecture)
3. [miniCurl — HTTP Requests](#minicurl--http-requests)
4. [Packet — Binary Serialization](#packet--binary-serialization)
5. [RP2P — Reliable Peer-to-Peer Protocol](#rp2p--reliable-peer-to-peer-protocol)
6. [Synchronizer — Multiplayer Lockstep](#synchronizer--multiplayer-lockstep)
7. [Deterministic Randomness](#deterministic-randomness)
8. [Key Files](#key-files)

---

## Overview

The network system is split into two independent subsystems:

- **HTTP** (`miniCurl`): used only for version checking at startup, via libcurl over the 3DS SOC service.
- **Local multiplayer** (`RP2P` + `Synchronizer`): local wireless play between multiple 3DS units, based on a deterministic lockstep model.

These two subsystems share no code.

---

## Layered Architecture

```
┌─────────────────────────────────┐
│  version.c                      │  version checking
│  miniCurl (HTTP / libcurl)      │
└────────────────┬────────────────┘
                 │ SOC (3DS network service)

┌─────────────────────────────────┐
│  Synchronizer                   │  lockstep, RNG, save transfer
│  ↕ application messages         │
│  RP2P                           │  reliability, sequencing, handshake
│  ↕ raw packets                  │
│  engine/network                 │  low-level 3DS network layer (local)
└─────────────────────────────────┘
```

---

## miniCurl — HTTP Requests

### Role

Performs HTTP GET requests using libcurl, after initializing the 3DS SOC service. Used exclusively to fetch the latest game version from the GitHub API.

### Initialization

```c
int internetInit();   // initializes SOC, returns 0 or -1
int exitInternet();   // releases SOC and the allocated buffer
```

`internetInit` allocates a 1 MB aligned buffer for the SOC service and checks the return value of `socInit`. On failure, the buffer is freed and `-1` is returned.

### Request

```c
char *miniCurlGet(const char *url);
```

- Configures libcurl with `CURLOPT_CAINFO = "romfs:/cacert.pem"` to validate HTTPS certificates.
- Returns a heap-allocated string containing the response body. The caller is responsible for `free`.
- Returns `NULL` on error.

### Receive Buffer

The response is accumulated dynamically via `write_chunk` and `realloc` as curl delivers chunks.

### Dependencies

| Dependency | Role |
|---|---|
| `libcurl` | HTTP/HTTPS request |
| `socInit` / `socExit` | 3DS network service |
| `romfs:/cacert.pem` | CA certificates for HTTPS |

---

## Packet — Binary Serialization

### Role

Provides read/write functions for primitive types into a binary buffer. Used by `Synchronizer` to build and parse network messages.

### API

Each function advances the buffer pointer and updates the size counter:

```c
// Write — returns pointer after the written element
void *writeBool  (void *buffer, uShort *size, bool   value);
void *writeUByte (void *buffer, uShort *size, uByte  value);
void *writeUShort(void *buffer, uShort *size, uShort value);
void *writeSInt  (void *buffer, uShort *size, sInt   value);

// Read — returns pointer after the read element, decrements size
void *readBool  (void *buffer, uShort *size, bool   *value);
void *readUByte (void *buffer, uShort *size, uByte  *value);
void *readUShort(void *buffer, uShort *size, uShort *value);
void *readSInt  (void *buffer, uShort *size, sInt   *value);
```

### Usage Example

```c
void *buffer = myBuffer;
uShort size = 0;
buffer = writeUByte(buffer, &size, SYNC_INPUTS);
buffer = writeSInt (buffer, &size, turnNumber);
// size now holds the total bytes written
```

---

## RP2P — Reliable Peer-to-Peer Protocol

### Role

Reliable transport layer on top of the 3DS local network. Handles:
- Packet sequencing and ordering
- Acknowledgements (ACK) and retransmission
- Host/client handshake and player index assignment
- Circular send buffer with a dedicated send thread

### Internal States

```
rp2pUninitialzed → rp2pInitHost / rp2pInitClient
                            ↓
                      rp2pBeforeStart
                            ↓
                        rp2pActive
```

### Handshake

**Host side:**
1. Sends a PING on connection to collect client IDs via ACKs
2. Once all players are detected (`playerCount == networkGetNodeCount()`), sends a START packet with the player ID/index mapping
3. Transitions to `rp2pBeforeStart` then `rp2pActive`

**Client side:**
1. Waits for a START packet from the host
2. Reads the ID list to determine its own player index
3. Transitions to `rp2pBeforeStart` then `rp2pActive`

### Packet Format

#### Header (7 bytes)
| Offset | Type | Content |
|---|---|---|
| 0 | `uShort` | Sequence number |
| 2 | `uByte` | Message type |
| 3 | `sInt` | Sender ID |

#### Internal Message Types
| Constant | Value | Description |
|---|---|---|
| `RP2P_MESSAGE_TYPE_INTERNAL_ACK` | 0 | Acknowledgement |
| `RP2P_MESSAGE_TYPE_INTERNAL_PING` | 1 | Peer discovery |
| `RP2P_MESSAGE_TYPE_INTERNAL_START` | 2 | Session start |
| `RP2P_MESSAGE_TYPE_APP` | 64 | Application message (Synchronizer) |

### Circular Send Buffer

The send buffer (`SENDBUFFERSIZE = (NETWORK_MAXDATASIZE + 256) * 10`) is managed as a ring with wrap handling. A dedicated thread (`handleSend`) drains pending packets every 10 ms. Packets acknowledged by all peers are removed via `clearSendAckedBuffer`.

### Public API

```c
void  rp2pInit(bool isHost, sInt localID, Callback onStart, MessageHandler mh);
void  rp2pTick();
void  rp2pExit();
bool  rp2pIsActive();

void  rp2pRecvRaw(void *packet, uShort size);  // called by engine/network
void  rp2pSend(void *message, uShort size);
void  rp2pFlush();                             // blocks until send buffer is empty

uByte rp2pGetPlayerCount();
uByte rp2pGetLocalPlayerIndex();
sInt  rp2pGetPlayerID(uByte playerIndex);
```

---

## Synchronizer — Multiplayer Lockstep

### Role

Implements a **deterministic lockstep** model: a game tick is only executed once the inputs of all players are known for that turn. This ensures all game instances evolve identically without needing to synchronize world state.

Single-player mode also goes through the Synchronizer (uniform code path).

### Lockstep Principle

```
Turn N:
  1. Each player sends their local inputs
  2. Synchronizer waits to receive inputs from all players
  3. When all received → seed RNG → game tick → compute next seed
  4. Turn N+1
```

`SYNCHRONIZER_TICKS_PER_TURN = 2`: the game tick is only triggered every 2 calls to `synchronizerTick`.

### Starting a Multiplayer Session

1. Host and client call `synchronizerStartMPHost()` / `synchronizerStartMPClient()` respectively
2. RP2P performs the handshake
3. `synchronizerHandleStart()` is called as a callback:
   - Host transfers the save file via `synchronizerSendSaveFile()` (chunks of `RP2P_MAXDATASIZE`)
   - Host sends a SYNC_START with the initial seed
   - Clients receive the save into `tmpTransfer.bin`, then receive SYNC_START
4. `synchronizerLoadWorld()` starts the game on all instances

### Application Message Types

| Constant | Value | Description |
|---|---|---|
| `SYNC_FILE_START` | 1 | Start of save transfer (total size) |
| `SYNC_FILE_DATA` | 2 | Save data chunk |
| `SYNC_START` | 3 | Game start (initial seed) |
| `SYNC_INPUTS` | 4 | A player's inputs for a given turn |

### Public API

```c
void synchronizerStartSP();           // start in single-player
void synchronizerStartMPHost();       // start as host
void synchronizerStartMPClient();     // start as client

bool synchronizerIsRunning();
void synchronizerTick(void (*gtick)(void));  // call every frame
void synchronizerReset();             // reset and disconnect
```

---

## Deterministic Randomness

For all instances to produce identical results, the random generator (`syncRand`) is:
- Seeded identically at the start of each turn (seed computed from the previous turn)
- Protected against out-of-turn calls: returns `0` if called outside a valid tick

```c
sInt   syncRand();       // integer in [0, SYNC_RAND_MAX]
double syncGaussRand();  // gaussian distribution (Box-Muller)
```

> `syncRand()` returns `0` if called outside a valid turn. All gameplay code must use `syncRand()` and not `rand()` to guarantee determinism.

---

## Key Files

| File | Role |
|---|---|
| `source/network/miniCurl.c/.h` | HTTP requests via libcurl, SOC init/exit |
| `source/network/Packet.c/.h` | Binary message serialization |
| `source/network/RP2P.c/.h` | Reliable peer-to-peer protocol, handshake, send buffer |
| `source/network/Synchronizer.c/.h` | Lockstep, save transfer, deterministic RNG |
| `source/engine/network.h` | Low-level 3DS network layer (local wireless) |
| `romfs/cacert.pem` | CA certificates for HTTPS requests |
