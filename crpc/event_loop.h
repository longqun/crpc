#ifndef CRPC_EVENT_LOOP_H
#define CRPC_EVENT_LOOP_H

#include <functional>
#include <list>
#include "crpc_timer.h"
#include "mutex.h"
#include "poller.h"

namespace crpc
{

typedef std::function<void (poll_event *event)> fd_create_cb;
typedef std::function<void (poll_event *event)> fd_event_cb;
typedef std::function<void (int fd)> fd_close_cb;


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

    void mod_fd_event(int fd, int event);

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
        {
            MutexGuard guard(_pending_mutex);
            _pending_functors.push_back(fun);
        }

        wake_up();
    }

    void run_at(int time, const functor& func);

    void run_every(int time, const functor& func);

    EPoller &get_poll()
    {
        return _poller;
    }

private:

    void run_every_internal(int time, const functor& func);
    void run_internal(int time, const functor& func);

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

    poll_event _wake_event;
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
