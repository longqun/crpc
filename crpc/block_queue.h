#ifndef CRPC_BLOCK_QUEUE_H
#define CRPC_BLOCK_QUEUE_H

#include <list>
#include "mutex.h"

namespace crpc
{

template<typename T>
class BlockQueue
{
public:
    BlockQueue()
    {}

    void push(const T& val)
    {
        MutexGuard guard(_mutex);
        _block_list.push_back(val);
    }

    bool pop(T* val)
    {
        MutexGuard guard(_mutex);
        if (_block_list.empty())
            return false;

        *val = _block_list.front();
        _block_list.pop_front();
        return true;
    }

private:
    std::list<T> _block_list;
    Mutex _mutex;
};

}

#endif
