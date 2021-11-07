#ifndef RPCCONTEXT_H
#define RPCCONTEXT_H
#include <execinfo.h>
#include <unordered_map>
#include <atomic>
#include <string>
#include "rpcmeta.pb.h"
#include "google/protobuf/service.h"
#include "google/protobuf/stubs/common.h"
#include "common_header.h"
#include "socket.h"
#include "io_buf.h"
#include "poller.h"
#include "crpc_log.h"
#include "non_copy.h"
#include "protocol.h"
#include "event_loop.h"

namespace crpc {

enum
{
    CONTEXT_NORMAL = 0,
    CONTEXT_CLOSE  = 1,
    CONTEXT_ERROR = 2
};

#define CAN_READ(status) !(status & DISABLE_READ)
#define CAN_WRITE(status) !(status & DISABLE_WRITE)
#define STATUS_ERROR(status) (status & CONTEX_ERROR)

class RpcContext : NonCopy
{
public:

    RpcContext(int fd, EventLoop* loop);

    ~RpcContext();

    int get_peer_port() const
    {
        return _peer_port;
    }

    std::string get_peer_ip() const
    {
        return _peer_ip;
    }

    void set_context_status(int status)
    {
        if (status == CONTEXT_ERROR)
            return;
        _con_status = status;
    }

    EventLoop* get_context_loop()
    {
        return _loop;
    }

    void set_proto_context(void *ptr)
    {
        _proto_ctx = ptr;
    }

    void *get_proto_context()
    {
        return _proto_ctx;
    }

    bool has_write_interest() const
    {
        return _poll_event->event & EPOLLOUT;
    }

    bool has_read_interest() const
    {
        return _poll_event->event & EPOLLIN;
    }

    void enable_read()
    {
        if (has_read_interest())
            return;

        _poll_event->event |= EPOLLIN;
        _loop->get_poll().mod_event(_poll_event);
    }

    void disable_read()
    {
         if (!has_read_interest())
            return;

        _poll_event->event &= ~EPOLLIN;
        _loop->get_poll().mod_event(_poll_event);
    }

    void enable_write()
    {
        if (has_write_interest())
            return;

        _poll_event->event |= EPOLLOUT;
        _loop->get_poll().mod_event(_poll_event);
    }

    void disable_write()
    {
        if (!has_write_interest())
            return;

        _poll_event->event &= ~EPOLLOUT;
        _loop->get_poll().mod_event(_poll_event);
    }

    void context_write(const char *data, int size);

    void on_rpc_context_init();

    void add_ref()
    {
        ++_ref_cnt;
    }

    void dec_ref()
    {
        --_ref_cnt;
        if (_ref_cnt <= 0)
            delete this;
    }
private:

    void context_call_wrap(const functor &cb);

    void context_status();

    void context_handle(bool is_read);

    void context_write();

    //当要关闭的时候调用
    void context_close();

    //有读事件的时候调用
    void context_read();

    void flush_write_buf();

    EventLoop* _loop;

    //socket
    Socket _socket;

    //当前fd在poll中感兴趣的事件 如EPOLLIN EPOLLOUT
    poll_event *_poll_event;

    //读取 & 写入的 iobuf
    IoBuf _io_buf;
    IoBuf _write_io_buf;

    uint32_t _con_status;

    //对端ip
    std::string _peer_ip;

    //对端端口
    int _peer_port;

    //协议解析
    Protocol* _proto;

    //协议私有化指针
    void* _proto_ctx;

    int _ref_cnt;
};

}
#endif
