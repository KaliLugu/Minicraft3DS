# Threads — Nintendo 3DS

## Table of Contents

1. [Overview](#overview)
2. [Hardware Architecture](#hardware-architecture)
3. [Thread API (ctrulib)](#thread-api-ctrulib)
4. [Priorities and Scheduling](#priorities-and-scheduling)
5. [Synchronization](#synchronization)
6. [Usage in the Project](#usage-in-the-project)
7. [Common Patterns](#common-patterns)
8. [Limitations and Pitfalls](#limitations-and-pitfalls)

---

## Overview

The 3DS exposes a thread system via **ctrulib** (`<3ds.h>`). The project wraps the ctrulib API in an engine layer (`source/engine/`) under the names `mthreadCreate`, `mthreadJoin`, `mthreadSleep`.

Threads are already used in the project for multiplayer networking (`RP2P.c` — dedicated send thread).

---

## Hardware Architecture

### ARM11 Processor

The old/new 3DS includes an ARM11 MPCore:

| Console | Cores available to apps | Clock speed |
|---|---|---|
| 3DS / 2DS | 2 cores (core 0 + core 1) | ~268 MHz |
| New 3DS / New 2DS | 4 cores (core 0–3) | ~804 MHz |

> One additional core is reserved for the system (core 2 on old 3DS, cores 2+3 on New 3DS) and is not accessible to applications.

### Typical Core Assignment

```
Core 0 — main thread (game loop, render, tick)
Core 1 — background threads (network, I/O, heavy computation)
```

A thread launched on core 1 runs **in true parallel** with the main thread on core 0, without disturbing it. This is the ideal setup for blocking operations such as network requests.

---

## Thread API (ctrulib)

### Project Engine Wrapper

```c
// Creation
MThread mthreadCreate(void (*func)(void), int stackSize);

// Wait for completion
void mthreadJoin(MThread thread);

// Pause the current thread
void mthreadSleep(long long nanoseconds);
```

### Direct ctrulib API (if needed)

```c
#include <3ds.h>

Thread threadCreate(ThreadFunc entrypoint, void *arg, size_t stackSize,
                    int prio, int affinity, bool detached);
void threadJoin(Thread thread, u64 timeout_ns);
void threadFree(Thread thread);
void svcSleepThread(s64 nanoseconds);
```

### `affinity` Parameter (target core)

| Value | Core | Usage |
|---|---|---|
| `-2` | Default core (core 1) | Recommended for background threads |
| `0` | Core 0 | Main thread — avoid |
| `1` | Core 1 | Explicit background thread |
| `2` | Core 2 | System reserved |

---

## Priorities and Scheduling

Priorities range from **0 (highest) to 63 (lowest)**. The main thread typically runs at priority **0x30** (48).

```c
// Non-critical background thread (network, version check)
int prio = 0x3F;  // 63 — very low priority

// Light real-time background thread
int prio = 0x30;  // same priority as main — avoid on core 0
```

> **Rule:** a background thread on core 1 can have any priority without impacting the main thread, since they run on separate cores. On core 0, a priority higher than 0x30 would steal time from the game.

---

## Synchronization

### LightLock (lightweight mutex)

Used in `RP2P.c` to protect the send and receive buffers.

```c
LightLock lock;

LightLock_Init(&lock);
LightLock_Lock(&lock);
// critical section
LightLock_Unlock(&lock);
```

### Simple Shared Variable (bool/flag)

For simple cases where a background thread writes once and the main thread reads:

```c
static volatile bool _result  = false;
static volatile bool _done    = false;

// Background thread
void myThread() {
    _result = doSomething();
    _done   = true;
}

// Main thread
if (_done && _result) {
    // use the result
}
```

`volatile` tells the compiler not to cache the variable in a register — required for variables shared between threads.

---

## Usage in the Project

### Network Send Thread (RP2P.c)

```c
// Creation in rp2pInit()
rp2pData.sendThread = mthreadCreate(&handleSend, STACKSIZE);  // STACKSIZE = 8 KB

// Shutdown in rp2pExit()
rp2pData.state = rp2pUninitialzed;  // stop signal
mthreadJoin(rp2pData.sendThread);
```

The thread loops continuously, consuming the circular send buffer and sleeping 10 ms between each iteration:

```c
static void handleSend() {
    while (true) {
        if (rp2pData.state == rp2pUninitialzed)
            return;  // stop condition
        // ... send pending packets
        mthreadSleep(10 * 1000 * 1000);  // 10 ms in nanoseconds
    }
}
```

### Version Check (MenuTitle.c)

`getLatestRemoteVersion()` is launched in a background thread to avoid blocking the title menu:

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
    // Launch the thread only once
    static bool _threadStarted = false;
    if (!_threadStarted) {
        _threadStarted = true;
        mthreadCreate(&_versionCheckThread, 16 * 1024);
    }
    // ... rest of tick
}

void menuTitleRender(int screen, int width, int height) {
    if (screen == 0) {
        if (_versionChecked && _hasNewVersion)
            renderText("new update is available", 0, 232);
        // ... rest of render
    }
}
```

> Note: `_versionChecked` only becomes `true` when the result is ready — no race condition on `_hasNewVersion`.

---

## Common Patterns

### One-Shot Background Thread

A thread that performs a single task and exits — no need for `join` if using `detached = true` (direct ctrulib) or if polling `_done`.

```c
static void singleTask() {
    // ... work
}

// Launch
threadCreate(singleTask, NULL, 16 * 1024, 0x3F, -2, true);  // detached
```

### Looping Thread with Stop Signal

Pattern used by `RP2P.c`:

```c
static volatile bool _running = true;

static void loopThread() {
    while (_running) {
        // ... work
        mthreadSleep(10 * 1000 * 1000);
    }
}

// Clean shutdown
_running = false;
mthreadJoin(thread);
```

---

## Limitations and Pitfalls

### `socInit` and the Main Thread

`socInit` (3DS network service) must be called from the **main thread**. If `internetInit()` is called from a secondary thread, SOC initialization may fail silently. For a version-check thread, initialize SOC in the main thread before launching the thread.

### No Guaranteed Thread-Safe `malloc`

ctrulib's allocator is not necessarily thread-safe. Avoid concurrent `malloc`/`free` calls without a mutex, or allocate buffers before launching the thread.

### Stack Size

Too small → hard-to-diagnose crash. Reference values:
- Simple thread (computation, flag): **8 KB**
- Network thread (curl, buffers): **32–64 KB**
- Thread with JSON parsing: **32 KB minimum**

### `mthreadSleep` Blocks Only the Current Thread

```c
mthreadSleep(1000 * 1000 * 1000);  // 1 second — blocks ONLY this thread
```

The main thread continues running normally during this time.
