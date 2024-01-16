#ifdef __SWITCH__
#include "../engine.h"
#include <switch.h>

struct _lock {
    Mutex lock;
};

struct _thread {
    Thread *thread;
};

Lock lockCreate() {
    Lock lock = malloc(sizeof(_lock));
    mutexInit(&lock->lock);
    return lock;
}

void lockLock(Lock lock) {
    mutexLock(&lock->lock);
}

void lockUnlock(Lock lock) {
    mutexUnlock(&lock->lock);
}

void lockFree(Lock lock) {
    free(lock);
}

MThread mthreadCreate(void (*main)(void), sInt stackSize) {
    sInt prio = 0;
    svcGetThreadPriority(&prio, CUR_THREAD_HANDLE);
    MThread mthread = malloc(sizeof(_thread));
    threadCreate(&mthread->thread, main, NULL, NULL, 0x10000, 0x2C, -2);
    threadStart(&mthread->thread);
    return mthread;
}

void mthreadSleep(sInt nanos) {
    svcSleepThread(nanos);
}

void mthreadJoin(MThread mthread) {
    threadWaitForExit(mthread->thread);
    threadClose(mthread->thread);
    free(mthread);
}

#endif
