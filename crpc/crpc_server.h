#ifndef CRPC_SERVER_H
#define CRPC_SERVER_H

#include <queue>
#include <string>
#include <memory>
#include <list>
#include <unordered_map>
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

namespace crpc
{

//service related
class CRpcServer : NonCopy {
public:
    CRpcServer();

    ~CRpcServer();

    void start(const ServerOption& option, ProtoType type = RPC_PROTO);

    void add_service(::google::protobuf::Service* service);

    void add_http_service(::google::protobuf::Service* service, const std::string& url_path, const std::string& method);

    //TODO 有没有更加优雅的方式?
    void del_fd(int fd);

    //TODO 不应该提供这么多无关的功能
    bool steal(std::weak_ptr<RpcContext>* ptr);

    EPoller& get_poller()
    {
        return _poller;
    }

    ProtoType get_type() const
    {
        return _type;
    }

private:

    static void handle_sig_stop(int sig);

    void run();

    void handle_acceptor();

    void handle_del_list();

    //poller related
    Acceptor _acceptor;
    EPoller _poller;

    //work related
    int _work_q_size;
    WorkStealingQueue<std::weak_ptr<RpcContext>>* _work_q;

    //for thread wait notify
    FutextMutex* _futex_mutex;
    std::vector<WorkThread*> _thread;

   //rcpcontex related
    std::unordered_map<int, std::shared_ptr<RpcContext>> _context_map;

    //lock _del_fd
    Mutex _mutex;
    std::list<int> _del_fd;

    //service
    ServerOption _option;

    ProtoType _type;
};

}


#endif
