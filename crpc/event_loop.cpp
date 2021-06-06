#include "event_loop.h"


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

void read_all_fd(int fd)
{
    char buf[4096];
    while (read(fd, buf, sizeof(buf)) > 0);
}

EventLoop::EventLoop():_wake_fd(createEventfd()),_stop(false)
{
    _poller.add_fd(_wake_fd);
}

void EventLoop::run()
{
    _loop_thread_id = pthread_self();
    while (!_stop)
    {
        int length = _poller.poll();
        std::vector<struct epoll_event>& event_vec = _poller.get_event_vec();
        for (int i = 0; i < length; ++i)
        {
            int fd = event_vec[i].data.fd;
            if (fd == _wake_fd)
            {
                read_all_fd(_wake_fd);
                continue;
            }

            if (!_fd_event_cb)
            {
                crpc_log("no fd_event_cb close %d", fd);
                close(fd);
                continue;
            }

            _fd_event_cb(fd, event_vec[i].events);
        }

        //call functors
        std::list<functor> functor_list;
        {
            MutexGuard guard(_pending_mutex);
            swap(functor_list, _pending_functors);
        }

        for (auto& cb : functor_list)
        {
            cb();
        }
    }
}

void EventLoop::stop()
{
    _stop = true;
    wake_up();
}

void EventLoop::wake_up()
{
    uint64_t i = 1;
    ::write(_wake_fd, &i, sizeof(uint64_t));
}

void EventLoop::remove_fd(int fd)
{
    crpc_log("remove fd %d", fd);
    _poller.del_fd(fd);
    if (_fd_close_del)
        _fd_close_del(fd);
}

bool EventLoop::add_fd(int fd)
{
    crpc_log("add fd %d", fd);
    _poller.add_fd(fd);
    if (_fd_create_cb)
        _fd_create_cb(fd);
}

}
