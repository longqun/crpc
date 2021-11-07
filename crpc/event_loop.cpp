#include "event_loop.h"


namespace crpc
{

EventLoop::EventLoop(): _stop(false)
{
}

EventLoop::~EventLoop()
{
    crpc_log("destroy loop %d ", this);
}

void EventLoop::run()
{
    _loop_thread_id = pthread_self();
    s_wake_event.fd = createEventfd();
    s_wake_event.read_cb = std::bind(&EventLoop::call_pending_functor, this);
    _poller.add_event(&s_wake_event);

    while (!_stop)
    {
        int length = _poller.poll();
        std::vector<struct epoll_event>& event_vec = _poller.get_event_vec();
        for (int i = 0; i < length; ++i)
        {
            poll_event* cur_event = (poll_event*)event_vec[i].data.ptr;
            if (event_vec[i].events & (EPOLLIN | EPOLLERR | EPOLLHUP) && cur_event->read_cb)
                 cur_event->read_cb();


            if (event_vec[i].events & (EPOLLIN | EPOLLERR | EPOLLHUP) && cur_event->write_cb)
                cur_event->write_cb();
        }

        call_pending_functor();
    }
    crpc_log("loop %p stop! ", this);
}

void EventLoop::add_poll_event(poll_event *event)
{
    _poller.add_event(event);
}

void EventLoop::update_poll_event(poll_event *event)
{
    _poller.mod_event(event);
}

void EventLoop::remove_poll_event(poll_event *event)
{
    _poller.del_event(event);
}

void EventLoop::stop()
{
    _stop = true;
    if (!is_in_loop_thread())
        wake_up();
}

void EventLoop::wake_up()
{
    uint64_t i = 1;
    ::write(s_wake_event.fd, &i, sizeof(uint64_t));
}

void EventLoop::call_pending_functor()
{
    read_all_fd(s_wake_event.fd);
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
