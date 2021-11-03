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

EventLoop::EventLoop(): _stop(false)
{
    _wake_event.fd = createEventfd();
    _wake_event.event = EPOLLIN;

    _timer_event.fd = _timer.fd();
    _timer_event.event = EPOLLIN;

    
    _poller.add_event(&_wake_event);
    _poller.add_event(&_timer_event);
}

void EventLoop::run()
{
    _loop_thread_id = pthread_self();
    while (!_stop)
    {
        bool has_timer = false;
        int length = _poller.poll();
        std::vector<struct epoll_event>& event_vec = _poller.get_event_vec();
        for (int i = 0; i < length; ++i)
        {
            int fd = event_vec[i].data.fd;
            if (fd == _wake_event.fd)
            {
                read_all_fd(_wake_event.fd);
                continue;
            }

            if (fd == _timer.fd())
                has_timer = true;

            if (!_fd_event_cb)
            {
                crpc_log("no fd_event_cb close %d", fd);
                close(fd);
                continue;
            }

            _fd_event_cb((poll_event *)event_vec[i].data.ptr);
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

        //timer ÊÂ¼þ
        if (has_timer)
        {
            read_all_fd(_timer.fd());
            _timer.on_timer();
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
    ::write(_wake_event.fd, &i, sizeof(uint64_t));
}

void EventLoop::remove_fd(int fd)
{
    crpc_log("remove fd %d", fd);
    poll_event event;
    event.fd = fd;
    _poller.del_event(&event);
    if (_fd_close_del)
        _fd_close_del(fd);
}

bool EventLoop::add_fd(int fd)
{
    crpc_log("add fd %d", fd);
    poll_event *event = new poll_event;
    event->event = EPOLLIN;
    event->fd = fd;

    _poller.add_event(event);
    if (_fd_create_cb)
        _fd_create_cb(event);
    return true;
}

void EventLoop::run_every_internal(int time, const functor& func)
{
    _timer.run_every(time, func);
}

void EventLoop::run_internal(int time, const functor& func)
{
    _timer.run_at(time, func);
}

void EventLoop::run_at(int time, const functor& func)
{
    run_in_loop(std::bind(&EventLoop::run_internal, this, time, func));
}

void EventLoop::run_every(int time, const functor& func)
{
    run_in_loop(std::bind(&EventLoop::run_every_internal, this, time, func));
}

}
