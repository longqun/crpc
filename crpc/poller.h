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

class EPoller
{
public:
    EPoller();
    int add_fd(int fd);

    int del_fd(int fd);

    int mod_fd(int fd, int event);

    int poll();

    std::vector<struct epoll_event>& get_event_vec()
    {
        return _event_vec;
    }

    int poll_fd() const
    {
        return _poll_fd;
    }

private:
    int _poll_fd;
    std::vector<struct epoll_event> _event_vec;
};

}
#endif
