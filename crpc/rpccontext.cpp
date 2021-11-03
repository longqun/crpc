#include <sys/epoll.h>
#include <pthread.h>
#include "utills.h"
#include "crpc_log.h"
#include "rpccontext.h"
#include "crpc_server.h"
namespace crpc
{

RpcContext::RpcContext(int fd, EventLoop* loop, poll_event* event): _loop(loop), 
                                                                        _socket(fd),
                                                                        _poll_event(event),
                                                                        _con_status(CONTEXT_NORMAL),
                                                                        _proto(NULL),
                                                                        _proto_ctx(NULL),
                                                                        _ref_cnt(1)
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
    if (_proto)
        _proto->del_proto_ctx(_proto_ctx);
    delete _poll_event;
    close(_socket.fd());
}

void RpcContext::context_read()
{
    //TODO handle close / error event
    while (true && (_con_status != CONTEXT_ERROR && _con_status != CONTEXT_CLOSE))
    {
        int len = _io_buf.read_fd(_socket.fd());
        //peer close
        if (len == 0)
        {
            crpc_log("peer close ! %p %d %d", this, _peer_port, _socket.fd());
            _con_status |= CONTEXT_ERROR;
            break;
        }
        else if (len < 0)
        {
            if (errno == EAGAIN)
            {
                break;
            }

            crpc_log("not normal fd %d port %d read %d read %d write %d", _socket.fd(), _peer_port, errno, _io_buf.size(), _write_io_buf.size());
            _con_status |= CONTEXT_ERROR;
            break;
        }

        //识别协议
        if (!_proto)
        {
            _proto = select_proto(_io_buf);
            if (!_proto)
            {
                continue;
            }
            _proto_ctx = _proto->alloc_proto_ctx(this);
        }

        if(_proto->read_event(&_io_buf, this) == PARSE_FAILED)
        {
            set_context_status(CONTEXT_CLOSE);
        }
    }
}

void RpcContext::context_write(const char *data, int size)
{
    //出错不再写入了
    if (_con_status == CONTEXT_ERROR)
        return;

    if (_write_io_buf.size() == 0)
    {
        //just write
        int write_size = write(_socket.fd(), data, size);
        if (write_size < -1 && errno != EAGAIN)
        {
            //error handle
            _con_status = CONTEXT_ERROR;
            return;
        }

        //write not full
        int left = size - write_size;
        if (left)
        {
            _write_io_buf.append(&data[write_size], left);
            enable_write();
        }
        else
        {
            //add to loop for next_call
            _loop->run_in_loop(std::bind(&Protocol::write_event, _proto, this));
        }
    }
    else
    {
        //handle _write_io_buf first
        flush_write_buf();
        if (_con_status == CONTEXT_ERROR)
            return;

        if (!_write_io_buf.empty())
        {
            _write_io_buf.append(data, size);
            enable_write();
        }
        else
        {
            //call recursive
            context_write(data, size);
        }
    }
}

void RpcContext::context_close()
{
    crpc_log("loop remove fd %d", _socket.fd());
    _loop->remove_fd(_poll_event->fd);
}

void RpcContext::handle_event(poll_event *event)
{
    if (CONTEXT_NORMAL == _con_status)
    {
        //read
        if (event->event & (EPOLLIN | EPOLLERR | EPOLLHUP))
        {
            context_read();
        }

        //write
        if (event->event & (EPOLLOUT | EPOLLERR | EPOLLHUP))
        {
            flush_write_buf();
        }
    }

    //read / write 都有可能改变状态
    if (CONTEXT_ERROR == _con_status)
    {
        //直接关闭
        context_close();
    }
    else if (CONTEXT_CLOSE == _con_status)
    {

        if (_write_io_buf.size() == 0)
        {
            //没有要写的数据了
           context_close();
        }
    }
}

void RpcContext::flush_write_buf()
{
    //没有buf可写
    if (_write_io_buf.empty() && has_write_interest())
    {
        disable_write();
    }

    if (_write_io_buf.write_fd(_socket.fd(), _write_io_buf.size()) < 0 && errno != EAGAIN)
    {
        set_context_status(CONTEXT_ERROR);
    }
}

}
