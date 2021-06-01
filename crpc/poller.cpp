#include "poller.h"

namespace crpc
{

int createEventfd()
{
  int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
  if (evtfd < 0)
  {
    abort();
  }
  return evtfd;
}


const static int s_epoll_size = 10;

EPoller::EPoller():_wake_fd(createEventfd()),_poll_fd(epoll_create(s_epoll_size)),    _event_vec(s_epoll_size)
{
    add_fd(_wake_fd, false);
}

int EPoller::add_fd(int fd, bool one_shot)
{
    struct epoll_event ev = {0};
    ev.events = EPOLLIN | EPOLLET;
    if (one_shot)
        ev.events |=  EPOLLONESHOT;
    ev.data.fd = fd;
    return epoll_ctl(_poll_fd, EPOLL_CTL_ADD, fd, &ev);
}

int EPoller::reset_oneshot(int fd)
{
    epoll_event event = {0};
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLET | EPOLLONESHOT;
    return epoll_ctl(_poll_fd, EPOLL_CTL_MOD, fd, &event);
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
