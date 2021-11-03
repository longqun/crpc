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

     //协议解析
    ParseResult read_event(IoBuf* io_buf, RpcContext* context);

    ParseResult write_event(RpcContext* context);

private:
    //协议匹配
    virtual ParseResult proto_match(IoBuf* io_buf);

    //创建协议所需要的context
    virtual void *alloc_proto_ctx(RpcContext* context);

    //移除创建的context
    virtual void del_proto_ctx(void *context);

private:


    void fill_reply_data(::google::protobuf::Message* req_msg, ::google::protobuf::Message* resp_msg);
};

}

#endif
