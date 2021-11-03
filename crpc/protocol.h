#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "io_buf.h"
namespace crpc
{

class RpcContext;
class ProtoRpcController;

enum ParseResult
{
    NEED_NORE_DATA = 0,
    PARSE_FAILED = 1,
    PARSE_SUCCESS = 2
};

class Protocol
{
public:
    Protocol()
    {
    }

     virtual ~Protocol() {};

     //读事件
     virtual ParseResult read_event(IoBuf* in, RpcContext* context) = 0;

    //写事件
    virtual ParseResult write_event(RpcContext* context) = 0;

    //协议匹配
    virtual ParseResult proto_match(IoBuf* in) = 0;

    //创建协议所需要的context
    virtual void *alloc_proto_ctx(RpcContext* context) = 0;

    //移除创建的context
    virtual void del_proto_ctx(void *context) = 0;
};

Protocol *select_proto(IoBuf& buf);

}
#endif
