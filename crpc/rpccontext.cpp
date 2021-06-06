#include <sys/epoll.h>
#include <pthread.h>
#include "utills.h"
#include "crpc_log.h"
#include "rpccontext.h"
#include "crpc_server.h"

namespace crpc
{

RpcContext::RpcContext(int fd, EventLoop* loop):_loop(loop), _socket(fd),
                                                      _con_status(CONTEXT_NORMAL),
                                                      _proto(NULL),
                                                      _user_data(NULL)
{
    _socket.set_non_blocking();

    //获取对端ip & 端口
    struct sockaddr_in sa;
    char ipAddr[INET_ADDRSTRLEN];
    socklen_t len = sizeof(sa);
    getpeername(fd, (struct sockaddr *)&sa, &len);
    _peer_port = ntohs(sa.sin_port);
    _peer_ip = inet_ntop(AF_INET, &sa.sin_addr, ipAddr, sizeof(ipAddr));
    crpc_log("new context %p %d\n", this, _socket.fd());
}

RpcContext::~RpcContext()
{
    crpc_log("destroy context %p %d\n", this, _socket.fd());
    close(_socket.fd());
    delete _user_data;
}

void RpcContext::context_read()
{
    while (true)
    {
        int len = _io_buf.read_fd(_socket.fd());
        //peer close
        if (len == 0)
        {
            crpc_log("peer close ! %p %d %d", this, _peer_port, _socket.fd());
            _con_status |= DISABLE_READ;
            break;
        }
        else if (len < 0)
        {
            if (errno == EAGAIN)
            {
                break;
            }

            crpc_log("not normal fd %d port %d read %d read %d write %d", _socket.fd(), _peer_port, errno, _io_buf.size(), _write_io_buf.size());
            _con_status |= DISABLE_READ;
            break;
        }

        //识别协议
        if (!_proto)
        {
            _proto = select_proto(&_io_buf);
            if (!_proto)
            {
                crpc_log("select proto failed");
                continue;
            }

            _user_data = _proto->proto_new();
        }

        ParseResult result = _proto->parse(&_io_buf, _user_data);
        if (result == NEED_NORE_DATA)
        {
            continue;
        }
        else if (result == PARSE_FAILED)
        {
            _con_status |= CONTEX_ERROR;
            break;
        }

        _proto->process(_user_data);
        _proto->response(this, &_write_io_buf, _user_data);
        //write out?
        context_write();
    }
}

void RpcContext::context_write()
{
    while (true)
    {
        if (!_write_io_buf.size())
            return;
        if (_write_io_buf.write_fd(_socket.fd(), _write_io_buf.size()) <= 0)
        {
            if (errno != EAGAIN)
            {
                crpc_log("abnormal fd %d errno %d", _socket.fd(), errno);
                _con_status |= CONTEX_ERROR;
            }
            break;
        }
    }
}

void RpcContext::context_close()
{
    crpc_log("loop remove fd %d", _socket.fd());
    _loop->remove_fd(_socket.fd());
}

void RpcContext::handle_event(uint32_t event)
{
    _event = event;
    //read ?
    if (CAN_READ(_con_status) && _event & (EPOLLIN | EPOLLERR | EPOLLHUP))
    {
        context_read();
    }

    //write ?
    if (CAN_WRITE(_con_status) && _event & (EPOLLOUT | EPOLLERR | EPOLLHUP))
    {
        context_write();
    }

    _event = 0;
    //case1:读被关闭了 && 本地已经写完了数据
    //case2:写关闭了?
    if ((!CAN_READ(_con_status) && _write_io_buf.size() == 0) ||
        !CAN_WRITE(_con_status) || STATUS_ERROR(_con_status))
    {
        crpc_log("emit close %d %u", _con_status, _write_io_buf.size());
        context_close();
    }
}

}
