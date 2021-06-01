#include "crpc_server.h"
#include "futex_mutex.h"
#include "crpc_log.h"
#include "rpc_service.h"

namespace crpc
{

const static int s_work_q_size = 4;
const static int s_steal_q_size = 2048;

CRpcServer::CRpcServer():_acceptor(&_poller),
                            _work_q_size(s_work_q_size),
                            _work_q(new WorkStealingQueue<std::weak_ptr<RpcContext>>[_work_q_size]),
                            _futex_mutex(new FutextMutex[_work_q_size])
{
    _thread.resize(_work_q_size, NULL);
    for (int i = 0; i < _work_q_size; ++i)
    {
        _thread[i] = new WorkThread(this, &_futex_mutex[i], &_work_q[i]);
        _work_q[i].init(s_steal_q_size);
    }
}

CRpcServer::~CRpcServer()
{

}

void CRpcServer::handle_acceptor()
{
    int fd = -1;
    while ((fd = _acceptor.accept()) != -1)
    {
        //new rpccontext
        std::shared_ptr<RpcContext> ptr(new RpcContext(fd, this));
        _poller.add_fd(fd, true);
        _context_map[fd] = ptr;
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

bool CRpcServer::steal(std::weak_ptr<RpcContext>* ptr)
{
    for (int i = 0; i < _work_q_size; ++i)
    {
        if (_work_q[i].steal(ptr))
            return true;
    }
    return false;
}

void CRpcServer::del_fd(int fd)
{
    MutexGuard mutex(_mutex);
    _del_fd.push_back(fd);
    _poller.wakeup_poll();
}

void CRpcServer::handle_del_list()
{
    MutexGuard mutex(_mutex);

    for (auto& ref : _del_fd)
    {
        if (_poller.del_fd(ref) < 0)
        {
            crpc_log("del fd %d failed %d", ref, errno);
        }

        _context_map[ref].reset();
        _context_map.erase(ref);
    }
    _del_fd.clear();
}

void CRpcServer::start(const ServerOption& option, ProtoType type)
{
    signal(SIGPIPE, SIG_IGN);
    _option = option;
    _type = type;
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

    while (true)
    {
        int length = _poller.poll();
        std::vector<struct epoll_event>& event_vec = _poller.get_event_vec();
        std::list<int> wake_thread;

        for (int i = 0; i < length; ++i)
        {
            //handle acceptor
            if(event_vec[i].data.fd == _acceptor.fd())
            {
                handle_acceptor();
            }
            else if (event_vec[i].data.fd == _poller.wake_fd())
            {
                //起唤醒作用而已
                char buf[1024];
                while (read(event_vec[i].data.fd, buf, sizeof(buf)) >= 0);
                //处理需要close的fd
                handle_del_list();
            }
            else
            {
                auto itr = _context_map.find(event_vec[i].data.fd);
                if (itr == _context_map.end())
                {
                    crpc_log("BUG ");
                    abort();
                }
                itr->second->set_event(event_vec[i].events);

                //投递到任务队列中
                bool success = false;
                for (int j = 0; j < _work_q_size; ++j)
                {
                    int index = (i + j) % _work_q_size;
                    std::weak_ptr<RpcContext> wp = itr->second;
                    if (_work_q[index].push(wp))
                    {
                        success = true;
                        //notify
                        wake_thread.push_back(index);
                        break;
                    }
                    crpc_log("q %d full cur %zu size %zu", index, _work_q[index].volatile_size(), _work_q[index].capacity());
                }

                //TODO 这里要优化一下，不能用sleep
                if (!success)
                {
                    usleep(100);
                    --i;
                }
            }
        }

        //notify thread work!
        for (auto ref : wake_thread)
        {
            _futex_mutex[ref].signal();
        }

    }
}



}

