#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "io_buf.h"
namespace crpc
{

class RpcContext;
class ProtoRpcController;

enum ParseResult
{
    NEED_NORE_DATA,
    PARSE_FAILED,
    PARSE_SUCCESS
};

class Protocol
{
public:
    Protocol()
    {
    }

     ~Protocol() {};

    //协议匹配
    virtual ParseResult proto_match(IoBuf* io_buf) = 0;

    //创建协议所需要的context
    virtual void *alloc_proto_ctx() = 0;

    //移除创建的context
    virtual void del_proto_ctx(void *context) = 0;

    //协议解析
    virtual ParseResult proto_parse(IoBuf* io_buf, RpcContext* context) = 0;

    //回调函数
    virtual void proto_process(RpcContext* context) = 0;

    //协议回复数据
    virtual void proto_response(ProtoRpcController* con, RpcContext* context, IoBuf* io_buf) = 0;

    //协议回复数据完成回调，可以用来重置ctx状态。
    virtual void proto_finished(RpcContext* context) = 0;
};

Protocol *select_proto(IoBuf& buf);

}
#endif
