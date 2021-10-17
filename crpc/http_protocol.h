#ifndef HTTP_PROTOCOL_H
#define HTTP_PROTOCOL_H

#include <string>
#include <unordered_map>
#include "google/protobuf/service.h"
#include "google/protobuf/stubs/common.h"
#include "protocol.h"
#include "rpccontext.h"
#include "io_buf.h"

namespace crpc
{

//http协议解析
class HttpProtocol : public Protocol
{
public:
    HttpProtocol()
    {}

    ~HttpProtocol()
    {}

    //协议匹配
    virtual ParseResult proto_match(IoBuf* io_buf);

    //创建协议所需要的context
    virtual void *alloc_proto_ctx();

    //移除创建的context
    virtual void del_proto_ctx(void *context);

    //协议解析
    virtual ParseResult proto_parse(IoBuf* io_buf, RpcContext* context);

    //回调函数
    virtual void proto_process(RpcContext* context);

    //协议回复数据
    virtual void proto_response(ProtoRpcController* con, RpcContext* context, IoBuf* io_buf);

    //协议回复数据完成回调，可以用来重置ctx状态。
    virtual void proto_finished(RpcContext* context);

private:
    void fill_reply_data(::google::protobuf::Message* req_msg, ::google::protobuf::Message* resp_msg);
};

}

#endif
