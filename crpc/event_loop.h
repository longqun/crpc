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


//TODO ʵ��һ����ʱ��
class EventLoop
{
public:
    EventLoop();

    void run();

    void wake_up();

    void stop();

    //ֻ����loop�����̵߳���
    bool add_fd(int fd);

    //ֻ����loop�����̵߳���
    void remove_fd(int fd);

    //�����µ�fd����poll�����
    void reg_fd_create_cb(const fd_create_cb& cb)
    {
        _fd_create_cb = cb;
    }

    //��fd���¼��ᴥ��
    void reg_fd_event_cb(const fd_event_cb& cb)
    {
        _fd_event_cb = cb;
    }

    //��fd��poll�Ƴ�������
    void reg_fd_close_cb(const fd_close_cb& cb)
    {
        _fd_close_del = cb;
    }

    //��fun���뵽loopִ��
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

    //��fd���¼��ᴥ��
    fd_event_cb _fd_event_cb;

    //����ӵ�epoll�ɹ���ᴥ��
    fd_create_cb _fd_create_cb;

    //��Ҫclose fd֮ǰ�ᴥ��
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
