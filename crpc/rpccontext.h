#ifndef RPCCONTEXT_H
#define RPCCONTEXT_H
#include <unordered_map>
#include <atomic>
#include <string>
#include "rpcmeta.pb.h"
#include "proto_rpc_controller.h"
#include "google/protobuf/service.h"
#include "google/protobuf/stubs/common.h"
#include "common_header.h"
#include "socket.h"
#include "io_buf.h"
#include "poller.h"
#include "crpc_log.h"
#include "non_copy.h"
#include "protocol.h"

namespace crpc {

enum
{
    CONTEXT_NORMAL = 0,
    DISABLE_READ = 1,
    DISABLE_WRITE = 2,
    CONTEX_ERROR = 4
};

#define CAN_READ(status) !(status & DISABLE_READ)
#define CAN_WRITE(status) !(status & DISABLE_WRITE)
#define STATUS_ERROR(status) (status & CONTEX_ERROR)


class CRpcServer;

class RpcContext : NonCopy
{
public:

    RpcContext(int fd, CRpcServer* server);

    ~RpcContext()
    {
         //crpc_log("destroy context %p %d\n", this, _socket.fd());
        _socket.close();
        delete _user_data;
    }

    void set_event(uint32_t event)
    {
        _event = event;
    }

    void handle_event();;

    int fd() const
    {
        return _socket.fd();
    }

    uint32_t get_status() const
    {
        return _con_status;
    }

    int get_peer_port() const
    {
        return _peer_port;
    }

    std::string get_peer_ip() const
    {
        return _peer_ip;
    }

    void set_context_close()
    {
        //no read any more and try to send out data
        _con_status |= DISABLE_READ;
    }

private:
        void reset_to_oneshot();

    //��Ҫ�رյ�ʱ�����
    void context_close();

    //�ж��¼���ʱ�����
    void context_read();

    //��д�¼���ʱ�����
    void context_write();



    CRpcServer* _server;

    //socket
    Socket _socket;

    //���δ�����ʱ��
    uint32_t _event;

    //��ȡ & д��� iobuf
    IoBuf _io_buf;
    IoBuf _write_io_buf;

    uint32_t _con_status;

    //�Զ�ip
    std::string _peer_ip;
    //�Զ˶˿�
    int _peer_port;

    //Э�����
    Proto* _proto;

    //user_data�����proto��������Ӧ�Ľṹ��
    void* _user_data;
};

}
#endif
