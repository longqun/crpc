#ifndef CRPC_POLLER_H
#define CRPC_POLLER_H

#include <functional>
#include <vector>
#include <sys/eventfd.h>
#include <sys/epoll.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h> 
#include <assert.h>
#include <unordered_map>
#include "crpc_log.h"

namespace crpc
{
typedef std::function<void ()> fd_cb;

struct poll_event
{
    int fd;
    int event;
    bool stop;
    fd_cb read_cb;
    fd_cb write_cb;
    fd_cb close_cb;

    //默认对读感兴趣
    poll_event():fd(-1), event(EPOLLIN), stop(false)
    {}
};

class EPoller
{
public:
    EPoller();
    int add_event(poll_event *event);

    int del_event(poll_event *event);

    int mod_event(poll_event *event);

    int poll();

    std::vector<struct epoll_event>& get_event_vec()
    {
        return _event_vec;
    }

private:
    int _poll_fd;
    std::vector<struct epoll_event> _event_vec;

    //维护 fd -> event映射
    std::unordered_map<int, poll_event *> _fd_event_map;
};

}
#endif
