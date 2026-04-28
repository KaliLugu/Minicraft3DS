# Threads — Nintendo 3DS

## Table des matières

1. [Vue d'ensemble](#vue-densemble)
2. [Architecture matérielle](#architecture-matérielle)
3. [API threads (ctrulib)](#api-threads-ctrulib)
4. [Priorités et scheduling](#priorités-et-scheduling)
5. [Synchronisation](#synchronisation)
6. [Utilisation dans le projet](#utilisation-dans-le-projet)
7. [Patterns courants](#patterns-courants)
8. [Limitations et pièges](#limitations-et-pièges)

---

## Vue d'ensemble

La 3DS expose un système de threads via **ctrulib** (`<3ds.h>`). Le projet encapsule l'API ctrulib dans une couche moteur (`source/engine/`) sous les noms `mthreadCreate`, `mthreadJoin`, `mthreadSleep`.

Les threads sont utilisés dès maintenant dans le projet pour le réseau multijoueur (`RP2P.c` — thread d'envoi dédié).

---

## Architecture matérielle

### Processeur ARM11

La 3DS old/new embarque un ARM11 MPCore :

| Console | Cœurs disponibles pour les apps | Fréquence |
|---|---|---|
| 3DS / 2DS | 2 cœurs (core 0 + core 1) | ~268 MHz |
| New 3DS / New 2DS | 4 cœurs (core 0–3) | ~804 MHz |

> Un cœur supplémentaire est réservé au système (core 2 sur old 3DS, core 2+3 sur New 3DS) et n'est pas accessible aux applications.

### Répartition habituelle

```
Core 0 — thread principal (game loop, render, tick)
Core 1 — threads de fond (réseau, I/O, calculs lourds)
```

Un thread lancé sur core 1 s'exécute **en parallèle réel** du thread principal sur core 0, sans le perturber. C'est le cas idéal pour des opérations bloquantes comme des requêtes réseau.

---

## API threads (ctrulib)

### Enveloppe moteur du projet

```c
// Création
MThread mthreadCreate(void (*func)(void), int stackSize);

// Attente de fin
void mthreadJoin(MThread thread);

// Pause du thread courant
void mthreadSleep(long long nanosecondes);
```

### API ctrulib directe (si besoin)

```c
#include <3ds.h>

Thread threadCreate(ThreadFunc entrypoint, void *arg, size_t stackSize,
                    int prio, int affinity, bool detached);
void threadJoin(Thread thread, u64 timeout_ns);
void threadFree(Thread thread);
void svcSleepThread(s64 nanoseconds);
```

### Paramètre `affinity` (cœur cible)

| Valeur | Cœur | Usage |
|---|---|---|
| `-2` | Cœur par défaut (core 1) | Recommandé pour les threads de fond |
| `0` | Core 0 | Thread principal — éviter |
| `1` | Core 1 | Thread de fond explicite |
| `2` | Core 2 | Réservé système |

---

## Priorités et scheduling

Les priorités vont de **0 (la plus haute) à 63 (la plus basse)**. Le thread principal tourne généralement à priorité **0x30** (48).

```c
// Thread de fond non-critique (réseau, vérification de version)
int prio = 0x3F;  // 63 — très basse priorité

// Thread de fond temps-réel léger
int prio = 0x30;  // même priorité que le main — à éviter sur core 0
```

> **Règle :** un thread de fond sur core 1 peut avoir n'importe quelle priorité sans impacter le thread principal, puisqu'ils tournent sur des cœurs distincts. Sur core 0, une priorité plus haute qu'0x30 volerait du temps au jeu.

---

## Synchronisation

### LightLock (mutex léger)

Utilisé dans `RP2P.c` pour protéger les buffers d'envoi et de réception.

```c
LightLock lock;

LightLock_Init(&lock);
LightLock_Lock(&lock);
// section critique
LightLock_Unlock(&lock);
```

### Variable partagée simple (bool/flag)

Pour les cas simples où un thread écrit une seule fois et le thread principal lit :

```c
static volatile bool _résultat = false;
static volatile bool _terminé  = false;

// Thread de fond
void monThread() {
    _résultat = faireQuelqueChose();
    _terminé  = true;
}

// Thread principal
if (_terminé && _résultat) {
    // utiliser le résultat
}
```

`volatile` indique au compilateur de ne pas mettre la variable en cache registre — nécessaire pour les variables partagées entre threads.

---

## Utilisation dans le projet

### Thread d'envoi réseau (RP2P.c)

```c
// Création dans rp2pInit()
rp2pData.sendThread = mthreadCreate(&handleSend, STACKSIZE);  // STACKSIZE = 8 Ko

// Arrêt dans rp2pExit()
rp2pData.state = rp2pUninitialzed;  // signal d'arrêt
mthreadJoin(rp2pData.sendThread);
```

Le thread tourne en boucle, consomme le buffer d'envoi circulaire et dort 10 ms entre chaque itération :

```c
static void handleSend() {
    while (true) {
        if (rp2pData.state == rp2pUninitialzed)
            return;  // condition d'arrêt
        // ... envoyer les paquets en attente
        mthreadSleep(10 * 1000 * 1000);  // 10 ms en nanosecondes
    }
}
```

### Vérification de version (MenuTitle.c)

`getLatestRemoteVersion()` est lancée dans un thread de fond pour ne pas bloquer le menu titre :

```c
static volatile bool _versionChecked = false;
static volatile bool _hasNewVersion  = false;

static void _versionCheckThread() {
    char *v = getLatestRemoteVersion();
    _hasNewVersion = isNewerVersion(v);
    free(v);
    _versionChecked = true;
}

void menuTitleTick() {
    // Lancer le thread une seule fois
    static bool _threadStarted = false;
    if (!_threadStarted) {
        _threadStarted = true;
        mthreadCreate(&_versionCheckThread, 16 * 1024);
    }
    // ... reste du tick
}

void menuTitleRender(int screen, int width, int height) {
    if (screen == 0) {
        if (_versionChecked && _hasNewVersion)
            renderText("new update is available", 0, 232);
        // ... reste du rendu
    }
}
```

> Note : `_versionChecked` passe à `true` seulement quand le résultat est prêt — pas de race condition sur `_hasNewVersion`.

---

## Patterns courants

### Thread de fond one-shot

Thread qui fait une tâche unique et se termine — pas besoin de `join` si on utilise `detached = true` (ctrulib directe) ou si on poll `_terminé`.

```c
static void tacheUnique() {
    // ... travail
}

// Lancement
threadCreate(tacheUnique, NULL, 16 * 1024, 0x3F, -2, true);  // detached
```

### Thread en boucle avec signal d'arrêt

Pattern utilisé par `RP2P.c` :

```c
static volatile bool _running = true;

static void threadBoucle() {
    while (_running) {
        // ... travail
        mthreadSleep(10 * 1000 * 1000);
    }
}

// Arrêt propre
_running = false;
mthreadJoin(thread);
```

---

## Limitations et pièges

### `socInit` et le thread principal

`socInit` (service réseau 3DS) doit être appelé depuis le **thread principal**. Si `internetInit()` est appelé depuis un thread secondaire, l'initialisation SOC peut échouer silencieusement. Pour un thread de vérification de version, initialiser SOC dans le thread principal avant de lancer le thread.

### Pas de `malloc` thread-safe garanti

L'allocateur de ctrulib n'est pas nécessairement thread-safe. Éviter les `malloc`/`free` concurrents sans mutex, ou allouer les buffers avant de lancer le thread.

### Stack size

Trop petite → crash difficile à diagnostiquer. Valeurs de référence :
- Thread simple (calcul, flag) : **8 Ko**
- Thread réseau (curl, buffers) : **32–64 Ko**
- Thread avec parsing JSON : **32 Ko minimum**

### `mthreadSleep` bloque le thread courant, pas le jeu

```c
mthreadSleep(1000 * 1000 * 1000);  // 1 seconde — bloque UNIQUEMENT ce thread
```

Le thread principal continue de tourner normalement pendant ce temps.
