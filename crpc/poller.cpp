#include "poller.h"

namespace crpc
{
const static int s_epoll_size = 10;

EPoller::EPoller():_poll_fd(epoll_create(s_epoll_size)),    _event_vec(s_epoll_size)
{
}

int EPoller::add_event(poll_event *event)
{
    struct epoll_event ev;
    ev.events = event->event | EPOLLET;
    ev.data.fd = event->fd;
    ev.data.ptr = event;
    return epoll_ctl(_poll_fd, EPOLL_CTL_ADD, event->fd, &ev);
}

int EPoller::del_event(poll_event *event)
{
    return epoll_ctl(_poll_fd, EPOLL_CTL_DEL, event->fd, NULL);
}

int EPoller::mod_event(poll_event *event)
{
    struct epoll_event ev;
    ev.events = event->event | EPOLLET;
    ev.data.fd = event->fd;
    ev.data.ptr = event;
    return epoll_ctl(_poll_fd, EPOLL_CTL_MOD, event->fd, &ev);
}

#define MAX_EVENT_VEC 10000
int EPoller::poll()
{
    //TODO expand _event_vec
    int ret = epoll_wait(_poll_fd, &_event_vec[0], _event_vec.size(), -1);
    if (ret > 0 && 
        ret < MAX_EVENT_VEC && 
        (size_t)ret == _event_vec.size())
    {
        _event_vec.resize(_event_vec.size() << 1);
    }
    return ret;
}

}
