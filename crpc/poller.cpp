#include "poller.h"

namespace crpc
{
const static int s_epoll_size = 10;

EPoller::EPoller():_poll_fd(epoll_create(s_epoll_size)),    _event_vec(s_epoll_size)
{
}

int EPoller::add_fd(int fd)
{
    struct epoll_event ev = {0};
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = fd;
    return epoll_ctl(_poll_fd, EPOLL_CTL_ADD, fd, &ev);
}

int EPoller::del_fd(int fd)
{
    return epoll_ctl(_poll_fd, EPOLL_CTL_DEL, fd, NULL);
}

int EPoller::mod_fd(int fd, int event)
{
    struct epoll_event ev = {0};
    ev.events = event;
    ev.data.fd = fd;
    return epoll_ctl(_poll_fd, EPOLL_CTL_MOD, fd, &ev);
}

int EPoller::poll()
{
    //TODO expand _event_vec
    int ret = epoll_wait(_poll_fd, &_event_vec[0], _event_vec.size(), -1);
    if (ret == _event_vec.size())
    {
        _event_vec.resize(_event_vec.size() << 1);
    }
    return ret;
}

}
