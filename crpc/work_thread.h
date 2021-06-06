#ifndef WORK_THREAD_H
#define WORK_THREAD_H

#include <memory>
#include "rpccontext.h"
#include "futex_mutex.h"
#include "non_copy.h"
#include "block_queue.h"
#include "work_steal_queue.h"
#include "count_down_latch.h"
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
    WorkThread(EventLoop* loop, CountDownLatch* latch);

    void run();

    static void * start_thread(void *arg)
    {
        pthread_detach(pthread_self());
        WorkThread* thread = (WorkThread*)(arg);
        thread->run_internal();
        return NULL;
    }

private:
    void run_internal();
    CountDownLatch* _latch;
    EventLoop* _loop;
};

}


#endif
