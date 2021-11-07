#include "work_thread.h"
#include "futex_mutex.h"
#include "crpc_server.h"
namespace crpc{

WorkThread::WorkThread(EventLoop* loop, CountDownLatch* latch)
            : _latch(latch), _loop(loop)
{}


void WorkThread::run()
{
    pthread_t ntid;
    pthread_create(&ntid, NULL, WorkThread::start_thread, this);
}

void WorkThread::run_internal()
{
    _loop->run();
    _latch->count_down();
}

}
