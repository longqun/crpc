#include "crpc_server.h"
#include "futex_mutex.h"
#include "crpc_log.h"
#include "rpc_service.h"

namespace crpc
{

bool s_stop = false;
int s_wake_fd = createEventfd();

static void* handle_sig_stop(int)
{
	s_stop = true;
    int64_t i = 1;
    write(s_wake_fd, &i, sizeof(i));
    return NULL;
}

const static int s_work_q_size = 4;
const static int s_steal_q_size = 2048;

CRpcServer::CRpcServer():_work_q_size(s_work_q_size), _latch(_work_q_size)
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
    //wait for the latch
    _latch.wait();

    //all thread stop clean thread  & loop
    for (size_t i = 0; i < _work_q_size; ++i)
    {
        delete _thread[i];
        delete _loop[i];
    }

    _thread.clear();
    _loop.clear();
}

EventLoop* CRpcServer::get_next_loop()
{
    assert(_loop.size() > 0);
    _loop_index = (_loop_index + 1) % _loop.size();
    return _loop[_loop_index];
}


void CRpcServer::handle_acceptor(int accept_fd, int event)
{
    int fd = -1;
    while ((fd = _acceptor.accept()) != -1)
    {
        EventLoop* loop = get_next_loop();
        loop->run_in_loop(std::bind(&EventLoop::add_fd, loop, fd));
    }
}

void CRpcServer::add_service(::google::protobuf::Service* service)
{
    add_rpc_service(service);
}

void CRpcServer::add_http_service(::google::protobuf::Service* service, const std::string& url_path, const std::string& method)
{
    add_rpc_http_service(service, url_path, method);
}

void CRpcServer::start(const ServerOption& option)
{
    signal(SIGPIPE, SIG_IGN);
    //signal(SIGSTOP, handle_sig_stop);
    _main_loop.add_fd(_acceptor.fd());
    _option = option;
    _acceptor.init(option);
    run();
}

void CRpcServer::run()
{
    //run work thread
    for (int i = 0; i < _work_q_size; ++i)
    {
        _thread[i]->run();
    }

    _main_loop.reg_fd_event_cb(std::bind(&CRpcServer::handle_acceptor, this, std::placeholders::_1, std::placeholders::_2));
    _main_loop.run();
}

}

