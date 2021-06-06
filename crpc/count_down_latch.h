#ifndef CRPC_COUNT_DOWN_LATCH_H
#define CRPC_COUNT_DOWN_LATCH_H

#include "non_copy.h"
#include "condition.h"
#include "mutex.h"

namespace crpc
{
class CountDownLatch : NonCopy
{
public:
    CountDownLatch(uint32_t count):_count(count),_cond(_mutex)
    {
    }

    void count_down()
    {
        MutexGuard guard(_mutex);
        if (--_count <= 0)
            _cond.notify_all();
    }

    void wait()
    {
        MutexGuard guard(_mutex);
        while (_count > 0)
        {
            _cond.wait();
        }
    }

private:
    Mutex _mutex;
    uint32_t _count;
    Condition _cond;
    
};

}
#endif
