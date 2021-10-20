#ifndef CRPC_SERVER_H
#define CRPC_SERVER_H

#include <queue>
#include <string>
#include <memory>
#include <list>
#include <unordered_map>
#include <functional>
#include "mutex.h"
#include "socket.h"
#include "acceptor.h"
#include "rpccontext.h"
#include "futex_mutex.h"
#include "work_steal_queue.h"
#include "work_thread.h"
#include "non_copy.h"
#include "block_queue.h"
#include "common_header.h"
#include "count_down_latch.h"
#include "event_loop.h"

namespace crpc
{

//service related
class CRpcServer : NonCopy {
public:
    CRpcServer();

    ~CRpcServer();

    void start(const ServerOption& option);

    void add_service(::google::protobuf::Service* service);

    void add_http_service(::google::protobuf::Service* service, const std::string& url_path, const std::string& method);

    void timer_run_at(int time, EventLoop* loop, const functor & func);

    void timer_run_every(int time, EventLoop* loop, const functor & func);

    EventLoop* get_main_loop()
    {
        return &_main_loop;
    }

private:

    void run();

    void handle_acceptor(int accept_fd, int event);

    EventLoop* get_next_loop();

    size_t _loop_index;

    //poller related
    Acceptor _acceptor;

    //work related
    int _work_q_size;

    //for thread wait notify
    std::vector<WorkThread*> _thread;
    std::vector<EventLoop*> _loop;

    //service
    ServerOption _option;

    EventLoop _main_loop;

    CountDownLatch _latch;
};

}


#endif
