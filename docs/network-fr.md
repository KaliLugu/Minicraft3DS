# Système réseau — Minicraft 3DS

## Table des matières

1. [Vue d'ensemble](#vue-densemble)
2. [Architecture en couches](#architecture-en-couches)
3. [miniCurl — Requêtes HTTP](#minicurl--requêtes-http)
4. [Packet — Sérialisation binaire](#packet--sérialisation-binaire)
5. [RP2P — Protocole fiable pair-à-pair](#rp2p--protocole-fiable-pair-à-pair)
6. [Synchronizer — Lockstep multijoueur](#synchronizer--lockstep-multijoueur)
7. [Aléatoire déterministe](#aléatoire-déterministe)
8. [Fichiers clés](#fichiers-clés)

---

## Vue d'ensemble

Le système réseau est divisé en deux sous-systèmes indépendants :

- **HTTP** (`miniCurl`) : utilisé uniquement pour la vérification de version au démarrage, via libcurl sur le service SOC de la 3DS.
- **Multijoueur local** (`RP2P` + `Synchronizer`) : jeu local sans-fil entre plusieurs 3DS, basé sur un modèle lockstep déterministe.

Ces deux sous-systèmes ne partagent aucun code entre eux.

---

## Architecture en couches

```
┌─────────────────────────────────┐
│  version.c                      │  vérification de version
│  miniCurl (HTTP / libcurl)      │
└────────────────┬────────────────┘
                 │ SOC (service réseau 3DS)

┌─────────────────────────────────┐
│  Synchronizer                   │  lockstep, RNG, transfert de save
│  ↕ messages applicatifs         │
│  RP2P                           │  fiabilité, séquençage, handshake
│  ↕ paquets bruts                │
│  engine/network                 │  couche réseau bas niveau (3DS local)
└─────────────────────────────────┘
```

---

## miniCurl — Requêtes HTTP

### Rôle

Effectue des requêtes HTTP GET en utilisant libcurl, après avoir initialisé le service SOC de la 3DS. Utilisé exclusivement pour récupérer la dernière version du jeu depuis l'API GitHub.

### Initialisation

```c
int internetInit();   // initialise SOC, retourne 0 ou -1
int exitInternet();   // libère SOC et le buffer alloué
```

`internetInit` alloue un buffer de 1 Mo aligné pour le service SOC et vérifie le retour de `socInit`. En cas d'échec, le buffer est libéré et `-1` est retourné.

### Requête

```c
char *miniCurlGet(const char *url);
```

- Configure libcurl avec `CURLOPT_CAINFO = "romfs:/cacert.pem"` pour valider les certificats HTTPS.
- Retourne une chaîne allouée sur le heap contenant le corps de la réponse. L'appelant est responsable du `free`.
- Retourne `NULL` en cas d'erreur.

### Format du buffer de réception

La réponse est accumulée dynamiquement via `write_chunk` et `realloc` au fil des chunks curl.

### Dépendances

| Dépendance | Rôle |
|---|---|
| `libcurl` | Requête HTTP/HTTPS |
| `socInit` / `socExit` | Service réseau 3DS |
| `romfs:/cacert.pem` | Certificats CA pour HTTPS |

---

## Packet — Sérialisation binaire

### Rôle

Fournit des fonctions de lecture/écriture de types primitifs dans un buffer binaire. Utilisé par `Synchronizer` pour construire et parser les messages réseau.

### API

Chaque fonction avance le pointeur de buffer et met à jour le compteur de taille :

```c
// Écriture — retourne le pointeur après l'élément écrit
void *writeBool  (void *buffer, uShort *size, bool   value);
void *writeUByte (void *buffer, uShort *size, uByte  value);
void *writeUShort(void *buffer, uShort *size, uShort value);
void *writeSInt  (void *buffer, uShort *size, sInt   value);

// Lecture — retourne le pointeur après l'élément lu, décrémente size
void *readBool  (void *buffer, uShort *size, bool   *value);
void *readUByte (void *buffer, uShort *size, uByte  *value);
void *readUShort(void *buffer, uShort *size, uShort *value);
void *readSInt  (void *buffer, uShort *size, sInt   *value);
```

### Exemple d'utilisation

```c
void *buffer = myBuffer;
uShort size = 0;
buffer = writeUByte(buffer, &size, SYNC_INPUTS);
buffer = writeSInt (buffer, &size, turnNumber);
// size contient maintenant la taille totale écrite
```

---

## RP2P — Protocole fiable pair-à-pair

### Rôle

Couche de transport fiable au-dessus du réseau local 3DS. Gère :
- Numérotation et ordonnancement des paquets
- Acquittements (ACK) et retransmission
- Handshake host/client et assignation des index joueurs
- Buffer d'envoi circulaire avec thread dédié

### États internes

```
rp2pUninitialzed → rp2pInitHost / rp2pInitClient
                            ↓
                      rp2pBeforeStart
                            ↓
                        rp2pActive
```

### Handshake

**Côté host :**
1. Envoie un PING à la connexion pour collecter les IDs des clients via les ACKs
2. Une fois tous les joueurs détectés (`playerCount == networkGetNodeCount()`), envoie un paquet START avec la liste des IDs et index joueurs
3. Passe en `rp2pBeforeStart` puis `rp2pActive`

**Côté client :**
1. Attend un paquet START du host
2. Lit la liste des IDs pour connaître son propre index joueur
3. Passe en `rp2pBeforeStart` puis `rp2pActive`

### Format des paquets

#### Header (7 octets)
| Offset | Type | Contenu |
|---|---|---|
| 0 | `uShort` | Numéro de séquence |
| 2 | `uByte` | Type de message |
| 3 | `sInt` | ID de l'expéditeur |

#### Types de messages internes
| Constante | Valeur | Description |
|---|---|---|
| `RP2P_MESSAGE_TYPE_INTERNAL_ACK` | 0 | Acquittement |
| `RP2P_MESSAGE_TYPE_INTERNAL_PING` | 1 | Découverte des pairs |
| `RP2P_MESSAGE_TYPE_INTERNAL_START` | 2 | Lancement de la session |
| `RP2P_MESSAGE_TYPE_APP` | 64 | Message applicatif (Synchronizer) |

### Buffer d'envoi circulaire

Le buffer d'envoi (`SENDBUFFERSIZE = (NETWORK_MAXDATASIZE + 256) * 10`) est géré comme un anneau avec gestion du wrap. Un thread dédié (`handleSend`) consomme les paquets en attente toutes les 10 ms. Les paquets acquittés par tous les pairs sont retirés via `clearSendAckedBuffer`.

### API publique

```c
void  rp2pInit(bool isHost, sInt localID, Callback onStart, MessageHandler mh);
void  rp2pTick();
void  rp2pExit();
bool  rp2pIsActive();

void  rp2pRecvRaw(void *packet, uShort size);  // appelé par engine/network
void  rp2pSend(void *message, uShort size);
void  rp2pFlush();                             // attend que le buffer d'envoi soit vide

uByte rp2pGetPlayerCount();
uByte rp2pGetLocalPlayerIndex();
sInt  rp2pGetPlayerID(uByte playerIndex);
```

---

## Synchronizer — Lockstep multijoueur

### Rôle

Implémente un modèle **lockstep déterministe** : chaque tick de jeu n'est exécuté que lorsque les inputs de tous les joueurs sont connus pour ce tour. Cela garantit que toutes les instances du jeu évoluent de façon identique sans avoir besoin de synchroniser l'état du monde.

Le jeu solo passe également par le Synchronizer (chemin de code uniforme).

### Principe du lockstep

```
Tour N :
  1. Chaque joueur envoie ses inputs locaux
  2. Synchronizer attend de recevoir les inputs de tous
  3. Quand tous reçus → seed RNG → game tick → seed suivant calculé
  4. Tour N+1
```

`SYNCHRONIZER_TICKS_PER_TURN = 2` : le tick de jeu n'est déclenché que tous les 2 appels à `synchronizerTick`.

### Démarrage d'une session multijoueur

1. Host et client appellent respectivement `synchronizerStartMPHost()` / `synchronizerStartMPClient()`
2. RP2P effectue le handshake
3. `synchronizerHandleStart()` est appelé en callback :
   - Le host transfère la save via `synchronizerSendSaveFile()` (chunks de `RP2P_MAXDATASIZE`)
   - Le host envoie un SYNC_START avec la seed initiale
   - Les clients reçoivent la save dans `tmpTransfer.bin` puis reçoivent le SYNC_START
4. `synchronizerLoadWorld()` démarre la partie sur toutes les instances

### Messages applicatifs (types Synchronizer)

| Constante | Valeur | Description |
|---|---|---|
| `SYNC_FILE_START` | 1 | Début du transfert de save (taille totale) |
| `SYNC_FILE_DATA` | 2 | Chunk de données de la save |
| `SYNC_START` | 3 | Démarrage du jeu (seed initiale) |
| `SYNC_INPUTS` | 4 | Inputs d'un joueur pour un tour donné |

### API publique

```c
void synchronizerStartSP();           // démarre en solo
void synchronizerStartMPHost();       // démarre en tant que host
void synchronizerStartMPClient();     // démarre en tant que client

bool synchronizerIsRunning();
void synchronizerTick(void (*gtick)(void));  // à appeler à chaque frame
void synchronizerReset();             // réinitialise et déconnecte
```

---

## Aléatoire déterministe

Pour que toutes les instances produisent les mêmes résultats, le générateur aléatoire (`syncRand`) est :
- Seedé identiquement au début de chaque tour (seed calculée à partir du tour précédent)
- Protégé contre les appels hors-tour : retourne `0` si appelé en dehors d'un tick valide

```c
sInt   syncRand();       // entier dans [0, SYNC_RAND_MAX]
double syncGaussRand();  // distribution gaussienne (Box-Muller)
```

> `syncRand()` retourne `0` si appelé hors d'un tour valide. Tout code gameplay doit utiliser `syncRand()` et non `rand()` pour garantir le déterminisme.

---

## Fichiers clés

| Fichier | Rôle |
|---|---|
| `source/network/miniCurl.c/.h` | Requêtes HTTP via libcurl, init/exit SOC |
| `source/network/Packet.c/.h` | Sérialisation binaire des messages |
| `source/network/RP2P.c/.h` | Protocole fiable pair-à-pair, handshake, buffer d'envoi |
| `source/network/Synchronizer.c/.h` | Lockstep, transfert de save, RNG déterministe |
| `source/engine/network.h` | Couche réseau bas niveau 3DS (local wireless) |
| `romfs/cacert.pem` | Certificats CA pour les requêtes HTTPS |
