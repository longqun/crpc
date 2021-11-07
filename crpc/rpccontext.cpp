#include <sys/epoll.h>
#include <pthread.h>
#include "utills.h"
#include "crpc_log.h"
#include "rpccontext.h"
#include "crpc_server.h"
namespace crpc
{

#define WRAP_BIND(cb)                                                                       \
    do {                                                                                    \
        add_ref();                                                                          \
        _loop->queue_in_loop(std::bind(&RpcContext::context_call_wrap, this, cb));          \
    } while(0)

RpcContext::RpcContext(int fd, EventLoop* loop): _loop(loop), 
                                                                        _socket(fd),
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
    close(_socket.fd());
    delete _poll_event;
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

//包装一下异步调用
void RpcContext::context_call_wrap(const functor &cb)
{
    cb();
    dec_ref();
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
            functor f = std::bind(&Protocol::write_event, _proto, this);
            WRAP_BIND(f);
        }
    }
    else
    {
        //handle _write_io_buf first
        context_write();
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
    dec_ref();
}

void RpcContext::context_write()
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

void RpcContext::context_handle(bool is_read)
{
    if (is_read)
        context_read();
    else
        context_write();

    context_status();
}

void RpcContext::context_status()
{
    if (_con_status == CONTEXT_CLOSE || _con_status == CONTEXT_ERROR)
    {
        if (_poll_event->stop)
            return;

        //标志位，关闭的标志位，避免重入
        _poll_event->stop = true;
        
        crpc_log("loop remove fd %d", _socket.fd());

        //引用计数
        functor f = std::bind(&EventLoop::remove_poll_event, _loop, _poll_event);
        WRAP_BIND(f);
    }
}

void RpcContext::on_rpc_context_init()
{
    _poll_event = new poll_event;
    _poll_event->fd = _socket.fd();
    _poll_event->event = EPOLLIN;
    _poll_event->read_cb = std::bind(&RpcContext::context_handle, this, true);
    _poll_event->write_cb = std::bind(&RpcContext::context_handle, this, false);
    _poll_event->close_cb = std::bind(&RpcContext::context_close, this);
    _loop->add_poll_event(_poll_event);
}

}
