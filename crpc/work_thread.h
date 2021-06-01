#ifndef WORK_THREAD_H
#define WORK_THREAD_H

#include <memory>
#include "rpccontext.h"
#include "futex_mutex.h"
#include "non_copy.h"
#include "block_queue.h"
#include "work_steal_queue.h"
namespace crpc{

class CRpcServer;

//TODO 抽象成一个工作线程，跟其他的耦合了
enum WorkStatus
{
    NOT_RUN,
    RUNNING,
    STOPING,
    STOPED
};

class WorkThread : NonCopy
{
public:
    WorkThread(CRpcServer* server, FutextMutex* mutex, WorkStealingQueue<std::weak_ptr<RpcContext>>* q);

    void run();

    void stop()
    {
        _status = STOPING;
    }

    bool stoped() const
    {
        return _status == STOPED;
    }

    static void * start_thread(void *arg)
    {
        pthread_detach(pthread_self());
        WorkThread* thread = (WorkThread*)(arg);
        thread->run_internal();
        return NULL;
    }

private:
    void run_internal();

    bool peek(std::weak_ptr<RpcContext> *ptr);

    WorkStatus _status;
    CRpcServer* _server;
    FutextMutex* _futex_mutex;
    WorkStealingQueue<std::weak_ptr<RpcContext>>* _work_q;
};

}


#endif
