#ifndef CRPC_CONDITION_H
#define CRPC_CONDITION_H

#include <pthread.h>
#include "mutex.h"

namespace crpc
{

class Condition
{
public:
    Condition(Mutex& mutex):_mutex(mutex)
    {
        pthread_cond_init(&_cond, NULL);
    }

    ~Condition()
    {
        pthread_cond_destroy(&_cond);
    }

    void wait()
    {
        pthread_cond_wait(&_cond, &_mutex._mutex);
    }

    void notify()
    {
        pthread_cond_signal(&_cond);
    }

    void notify_all()
    {
        pthread_cond_broadcast(&_cond);
    }

private:
    pthread_cond_t _cond;
    Mutex& _mutex;

};

}

#endif
