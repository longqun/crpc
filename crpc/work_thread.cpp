#include "work_thread.h"
#include "futex_mutex.h"
#include "crpc_server.h"
#include "work_steal_queue.h"

namespace crpc{

WorkThread::WorkThread(CRpcServer* server, FutextMutex* mutex, WorkStealingQueue<std::weak_ptr<RpcContext>>* q)
            : _status(WorkStatus::NOT_RUN), _server(server), _futex_mutex(mutex), _work_q(q)
{}

bool WorkThread::peek(std::weak_ptr<RpcContext> *ptr)
{
    if (_work_q->steal(ptr))
    {
        return true;
    }

    return _server->steal(ptr);
}

void WorkThread::run()
{
    if (_status != WorkStatus::NOT_RUN)
    {
        crpc_log("status %d", _status);
        return;
    }

    pthread_t ntid;
    pthread_create(&ntid, NULL, WorkThread::start_thread, this);
}

void WorkThread::run_internal()
{
    _status = WorkStatus::RUNNING;
    while (_status != WorkStatus::STOPING)
    {
        _futex_mutex->wait();
        std::weak_ptr<RpcContext> ptr;
        while (peek(&ptr))
        {
            std::shared_ptr<RpcContext> share_ptr = ptr.lock();
            if (!share_ptr)
            {
                crpc_log("share_ptr has destroyed");
                continue;
            }

            share_ptr->handle_event();
        }
    }

    _status = WorkStatus::STOPED;
}


}
