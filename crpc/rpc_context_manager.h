#ifndef CRPC_RPC_CONTEXT_MANAGER_H
#define CRPC_RPC_CONTEXT_MANAGER_H

#include <execinfo.h>
#include <list>
#include <unordered_map>
#include <stdlib.h>
#include "crpc_log.h"
#include "rpccontext.h"
#include "event_loop.h"

namespace crpc
{

class RpcContextManager
{
public:
    RpcContextManager(EventLoop* loop):_loop(loop)
    {}

    RpcContextManager()
    {
        for (auto itr = _manger_map.begin(); itr != _manger_map.end(); ++itr)
        {
            delete itr->second;
        }

        _manger_map.clear();
    }

    void add_context(poll_event *event)
    {
        if (_manger_map.find(event->fd) != _manger_map.end())
        {
            crpc_log("add exist fd %d", event->fd);
            abort();
            return;
        }

        _manger_map[event->fd] = new RpcContext(event->fd, _loop, event);
    }

    void remove_context(int fd)
    {
        if (_manger_map.find(fd) == _manger_map.end())
            return;

        _manger_map[fd]->dec_ref();
        _manger_map.erase(fd);
    }

    void handle_context_event(poll_event *event)
    {
        auto itr = _manger_map.find(event->fd);
        if (itr == _manger_map.end())
            return;

         itr->second->handle_event(event);
    }

private:
    std::unordered_map<int, RpcContext*> _manger_map;
    EventLoop* _loop;

};


}

#endif
