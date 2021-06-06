#ifndef CRPC_EVENT_LOOP_H
#define CRPC_EVENT_LOOP_H

#include <functional>
#include <list>
#include "mutex.h"
#include "poller.h"

namespace crpc
{

typedef std::function<void (int fd)> fd_create_cb;
typedef std::function<void (int fd, int event)> fd_event_cb;
typedef std::function<void (int fd)> fd_close_cb;
typedef std::function<void ()> functor;


//TODO 实现一个定时器
class EventLoop
{
public:
    EventLoop();

    void run();

    void wake_up();

    void stop();

    //只能在loop所在线程调用
    bool add_fd(int fd);

    //只能在loop所在线程调用
    void remove_fd(int fd);

    //当有新的fd加入poll会调用
    void reg_fd_create_cb(const fd_create_cb& cb)
    {
        _fd_create_cb = cb;
    }

    //当fd有事件会触发
    void reg_fd_event_cb(const fd_event_cb& cb)
    {
        _fd_event_cb = cb;
    }

    //当fd从poll移除被调用
    void reg_fd_close_cb(const fd_close_cb& cb)
    {
        _fd_close_del = cb;
    }

    //将fun移入到loop执行
    void run_in_loop(const functor& fun)
    {
        if (is_in_loop_thread())
        {
            fun();
            return;
        }

        {
            MutexGuard guard(_pending_mutex);
            _pending_functors.push_back(fun);
        }

        wake_up();
    }

private:

    bool is_in_loop_thread() const
    {
        return _loop_thread_id == pthread_self();
    }

    void handle_fd();

    //当fd有事件会触发
    fd_event_cb _fd_event_cb;

    //当添加到epoll成功后会触发
    fd_create_cb _fd_create_cb;

    //当要close fd之前会触发
    fd_close_cb _fd_close_del;

    int _wake_fd;
    bool _stop;

    Mutex _pending_mutex;
    std::list<functor> _pending_functors;

    EPoller _poller;

    pthread_t _loop_thread_id;

};

}
#endif
