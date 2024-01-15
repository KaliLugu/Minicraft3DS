#pragma once
#include "dtypes.h"

typedef struct _lock _lock;
typedef _lock *Lock;

typedef struct _thread _thread;
typedef _thread *MThread;

extern Lock lockCreate();
extern void lockLock(Lock lock);
extern void lockUnlock(Lock lock);
extern void lockFree(Lock lock);

extern MThread mthreadCreate(void (*main)(void), sInt stackSize);
extern void mthreadSleep(sInt nanos);
extern void mthreadJoin(MThread mthread);
