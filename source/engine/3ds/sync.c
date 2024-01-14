#ifdef __3DS__
#include "../engine.h"
#include <3ds.h>

struct _lock {
    LightLock lock;
};

struct _event {
    LightEvent event;
};

struct _thread {
    Thread thread;
};

Lock lockCreate() {
    Lock lock = linearAlloc(sizeof(_lock));
    LightLock_Init(&lock->lock);
    return lock;
}

void lockLock(Lock lock) {
    LightLock_Lock(&lock->lock);
}

void lockUnlock(Lock lock) {
    LightLock_Unlock(&lock->lock);
}

void lockFree(Lock lock) {
    linearFree(lock);
}

Event eventCreate() {
    Event event = linearAlloc(sizeof(_event));
    LightEvent_Init(&event->event, RESET_ONESHOT);
    return event;
}

void eventAwait(Event event) {
    LightEvent_Wait(&event->event);
}

void eventSignal(Event event) {
    LightEvent_Signal(&event->event);
}

void eventFree(Event event) {
    linearFree(event);
}

MThread mthreadCreate(void (*main)(void), sInt stackSize) {
    sInt prio = 0;
    svcGetThreadPriority(&prio, CUR_THREAD_HANDLE);
    Thread thread = threadCreate(main, NULL, stackSize, prio - 1, -2, false);
    MThread mthread = linearAlloc(sizeof(_thread));
    mthread->thread = thread;
    return mthread;
}

void mthreadSleep(sInt nanos) {
    svcSleepThread(nanos);
}

void mthreadJoin(MThread mthread) {
    threadJoin(mthread->thread, U64_MAX);
    threadFree(mthread->thread);
    linearFree(mthread);
}

#endif
