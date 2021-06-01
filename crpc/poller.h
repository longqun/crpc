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
    int add_fd(int fd, bool one_shot);

    int del_fd(int fd);

    int mod_fd(int fd, int event);

    int reset_oneshot(int fd);

    int poll();

    void wakeup_poll()
    {
        uint64_t i = 1;
        ::write(_wake_fd, &i, sizeof(uint64_t));
    }

    std::vector<struct epoll_event>& get_event_vec()
    {
        return _event_vec;
    }

    int wake_fd() const
    {
        return _wake_fd;
    }

    int poll_fd() const
    {
        return _poll_fd;
    }

private:
    int _wake_fd;
    int _poll_fd;
    std::vector<struct epoll_event> _event_vec;
};

}
#endif
