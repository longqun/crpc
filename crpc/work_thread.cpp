#include "work_thread.h"
#include "futex_mutex.h"
#include "crpc_server.h"
#include "rpc_context_manager.h"
#include "work_steal_queue.h"

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
    RpcContextManager manager(_loop);
    _loop->reg_fd_create_cb(std::bind(&RpcContextManager::add_context, &manager, std::placeholders::_1));
    _loop->reg_fd_close_cb(std::bind(&RpcContextManager::remove_context, &manager, std::placeholders::_1));
    _loop->reg_fd_event_cb(std::bind(&RpcContextManager::handle_context_event, &manager, std::placeholders::_1, std::placeholders::_2));
    _loop->run();
    _latch->count_down();
}

}
