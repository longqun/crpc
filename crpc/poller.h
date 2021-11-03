#ifndef CRPC_POLLER_H
#define CRPC_POLLER_H

#include <vector>
#include <sys/eventfd.h>
#include <sys/epoll.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h> 
#include "crpc_log.h"

namespace crpc
{
int createEventfd();


struct poll_event
{
    int fd;
    int event;

    poll_event():fd(-1), event(0)
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
};

}
#endif
