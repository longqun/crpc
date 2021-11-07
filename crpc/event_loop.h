#ifndef CRPC_EVENT_LOOP_H
#define CRPC_EVENT_LOOP_H

#include <functional>
#include <list>
#include "crpc_timer.h"
#include "mutex.h"
#include "poller.h"

namespace crpc
{

class EventLoop
{
public:
    EventLoop();

    ~EventLoop();

    void run();

    void wake_up();

    void stop();

    //将fun移入到loop执行
    void run_in_loop(const functor& fun)
    {
        if (is_in_loop_thread())
        {
            fun();
            return;
        }

        queue_in_loop(fun);
    }

    EPoller &get_poll()
    {
        return _poller;
    }

    void queue_in_loop(const functor& fun)
    {
        {
            MutexGuard guard(_pending_mutex);
            _pending_functors.push_back(fun);
        }
        wake_up();
    }

    void run_at(int time, const functor& func);

    void run_every(int time, const functor& func);

    void add_poll_event(poll_event *event);

    void update_poll_event(poll_event *event);

    void remove_poll_event(poll_event *event);

private:

    void run_every_internal(int time, const functor& func);
    void run_internal(int time, const functor& func);

    void call_pending_functor();

    bool is_in_loop_thread() const
    {
        return _loop_thread_id == pthread_self();
    }

    poll_event s_wake_event;
    bool _stop;

    Mutex _pending_mutex;
    std::list<functor> _pending_functors;

    EPoller _poller;

    pthread_t _loop_thread_id;

    poll_event _timer_event;
    CRpcTimer _timer;

};

}
#endif
