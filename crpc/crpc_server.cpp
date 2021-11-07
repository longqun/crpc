#include <sys/sysinfo.h>
#include "crpc_server.h"
#include "futex_mutex.h"
#include "crpc_log.h"

namespace crpc
{

bool s_stop = false;
int s_wake_fd = createEventfd();

static void handle_sig_stop(int)
{
    crpc_log("SIG STOP!");
	s_stop = true;
    int64_t i = 1;
    write(s_wake_fd, &i, sizeof(i));
}

CRpcServer::CRpcServer():_work_q_size(get_nprocs()), _latch(_work_q_size)
{
    for (size_t i = 0; i < _work_q_size; ++i)
    {
        EventLoop* loop = new EventLoop();
        _thread.push_back(new WorkThread(loop, &_latch));
        _loop.push_back(loop);
    }
}

CRpcServer::~CRpcServer()
{
    //all thread stop clean thread  & loop
    for (size_t i = 0; i < _work_q_size; ++i)
    {
        delete _thread[i];
        delete _loop[i];
    }

    _thread.clear();
    _loop.clear();
    crpc_log("call destroy server");
}

EventLoop* CRpcServer::get_next_loop()
{
    assert(_loop.size() > 0);
    _loop_index = (_loop_index + 1) % _loop.size();
    return _loop[_loop_index];
}


void CRpcServer::handle_acceptor(poll_event *)
{
    int fd = -1;
    while ((fd = _acceptor.accept()) != -1)
    {
        EventLoop* loop = get_next_loop();

        RpcContext *context = new RpcContext(fd, loop);
        loop->run_in_loop(std::bind(&RpcContext::on_rpc_context_init, context));
    }
}

void CRpcServer::stop()
{
    read_all_fd(_wake_event.fd);
    _main_loop.stop();
    for (size_t i = 0; i < _loop.size(); ++i)
    {
        _loop[i]->stop();
    }
    crpc_log("server will stop now!");
}

void CRpcServer::start(const ServerOption& option)
{
    _option = option;
    _acceptor.init(option);
    signal(SIGPIPE, SIG_IGN);
    signal(SIGTERM, handle_sig_stop);

    _accept_event.fd = _acceptor.fd();
    _accept_event.read_cb = std::bind(&CRpcServer::handle_acceptor, this, &_accept_event);
    _main_loop.add_poll_event(&_accept_event);

    _wake_event.fd = s_wake_fd;
    _wake_event.read_cb = std::bind(&CRpcServer::stop, this);
    _main_loop.add_poll_event(&_wake_event);

    run();
}

void CRpcServer::run()
{
    //run work thread
    for (size_t i = 0; i < _work_q_size; ++i)
    {
        _thread[i]->run();
    }

    _main_loop.run();
    _latch.wait();
    crpc_log("server stop sucess!");
}

}

