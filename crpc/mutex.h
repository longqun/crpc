#ifndef _CRPC_MUTEX_H
#define _CRPC_MUTEX_H

#include "non_copy.h"

namespace crpc {

class Mutex : NonCopy {
public:
    Mutex()
    {
        pthread_mutex_init(&_mutex, NULL);
    }

    ~Mutex()
    {
        pthread_mutex_destroy(&_mutex);
    }

    void lock()
    {
        pthread_mutex_lock(&_mutex);
    }

    void unlock()
    {
        pthread_mutex_unlock(&_mutex);
    }

private:
    pthread_mutex_t _mutex;

    friend class Condition;
};

class MutexGuard : NonCopy {
public:
    MutexGuard(Mutex& mutex):_mutex(mutex)
    {
        _mutex.lock();
    }

    ~MutexGuard()
    {
        _mutex.unlock();
    }

private:
    Mutex& _mutex;
};

}

#endif

