#ifndef __BASEIF_H_
#define __BASEIF_H_

#include <stdint.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdlib.h>
#include <pthread.h>

#include "log.h"

#ifdef __cplusplus
extern "C" {
#endif



#define WRAPOS_OK 0
#define WRAPOS_ERROR -1
#define WRAPOS_TIMEOUT -2



static void wrapSleep_ms(unsigned int v) {
	usleep(v * 1000);
}

static unsigned int wrapGetTime_ms() { // From epoh
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

static unsigned int wrapGetTicks_ms() { // From boot
	return wrapGetTime_ms(); // Actually it's not critical now
}

static uint32_t swapBytes32(uint32_t v) {
	return ((v & 0x0000FF00) << 8) | ((v & 0x00FF0000) >> 8) | ((v & 0x000000FF) << 24) | ((v & 0xFF000000) >> 24);
}

static uint64_t swapBytes40(uint64_t v) {
	return (v & 0x0000FF0000) | ((v & 0x000000FF00) << 16) | ((v & 0x00FF000000) >> 16) | ((v & 0x00000000FF) << 32) | ((v & 0xFF00000000) >> 32);
}

static void *wrapHeapAlloc(size_t size) {
	return malloc(size);
}

static void wrapHeapFree(void *ptr) {
	free(ptr);
}



int wrapFileLoad(const char *fn, void **buf, size_t *len);



typedef pthread_mutex_t wrapMutex_t;

static int wrapMutexInit(wrapMutex_t *m) {
	pthread_mutexattr_t attr;
	if(pthread_mutexattr_init(&attr) != 0) return WRAPOS_ERROR;
	if(pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE) != 0) return WRAPOS_ERROR;
	return (pthread_mutex_init(m, &attr) == 0) ? WRAPOS_OK : WRAPOS_ERROR;
}

static int wrapMutexDestroy(wrapMutex_t *m) {
	return (pthread_mutex_destroy(m) == 0) ? WRAPOS_OK : WRAPOS_ERROR;
}

static int wrapMutexLock(wrapMutex_t *m) {
	return (pthread_mutex_lock(m) == 0) ? WRAPOS_OK : WRAPOS_ERROR;
}

static int wrapMutexUnlock(wrapMutex_t *m) {
	return (pthread_mutex_unlock(m) == 0) ? WRAPOS_OK : WRAPOS_ERROR;
}



typedef void *wrapSem_t;

int wrapSemCreate(wrapSem_t *sem, unsigned int cnt);
int wrapSemDestroy(wrapSem_t *sem);
int wrapSemGive(wrapSem_t *sem);
int wrapSemTake(wrapSem_t *sem, int ms);



typedef void(*wrapThreadFunction_t)(void *pData);

typedef void *wrapThread_t;

int wrapThreadStart(wrapThread_t *thread, wrapThreadFunction_t func, void *pData);
int wrapThreadStop(wrapThread_t *thread);
static void wrapThreadExit(int result) {
//	pthread_detach(pthread_self);
	pthread_exit(NULL);
}

#ifdef __cplusplus
}

class wrapAtomicScope_t {
protected:
	pthread_mutex_t &_m;
public:
	wrapAtomicScope_t(pthread_mutex_t &m): _m(m) {pthread_mutex_lock(&_m);}
	~wrapAtomicScope_t() {pthread_mutex_unlock(&_m);}
};

#define WRAP_ATOMIC_SCOPE static pthread_mutex_t wrapAtomicScope_v = PTHREAD_RECURSIVE_MUTEX_INITIALIZER; wrapAtomicScope_t wrapAtomicScope(wrapAtomicScope_v);
#define WRAP_ATOMIC_SCOPE_M(M) wrapAtomicScope_t wrapAtomicScope(M);

#endif

#endif
