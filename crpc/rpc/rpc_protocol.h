#ifndef RPC_PROTOCAL_H
#define RPC_PROTOCAL_H

#include <string>
#include "io_buf.h"
#include "protocol.h"
#include "rpcmeta.pb.h"
#include "rpc_service.h"
#include "proto_rpc_controller.h"
#include "google/protobuf/service.h"
#include "google/protobuf/stubs/common.h"

namespace crpc
{

#define CRPC_STR "CRPC"
#define CRPC_STR_LEN (4)

//|request   ->    "crpc"(固定字符串)| rpc_meta大小（定长4字节)|rpc_meta序列化数据（不定长）|request序列化数据（不定长）
// response  ->    response_size(定长4字节)|response序列号数据（不定长）
//status 当前解析状态
enum RpcParseStatus
{
    INVALID_STATUS,
    INIT,
    AFTER_CRPC,
    META_SIZE,
    META_DATA,
};

struct rpc_header
{
    size_t rpc_meta_size;
    RpcMeta rpc_meta;
    //request
    std::string probuf_str;

    //parse status
    RpcParseStatus status;

    rpc_header():rpc_meta_size(0), status(INIT)
    {}

    void reset()
    {
        rpc_meta_size = 0;
        probuf_str = std::string();
        status = INIT;
    }
};

struct rpc_response
{
    size_t response_len;
    std::string proto_str;

    void reset()
    {
        response_len = 0;
        proto_str = std::string();
    }
};

//抽象出来专门用来解析rpc的
class RpcProtocol : public Protocol
{
private:
    struct RpcProtocolContext
    {
        rpc_header _header;
        rpc_response _response;
    };
public:
    RpcProtocol()
    {}

    ~RpcProtocol()
    {}

     //读事件
    ParseResult read_event(IoBuf* in, RpcContext* context);

    //写事件
    ParseResult write_event(RpcContext* context);

    //协议匹配
    ParseResult proto_match(IoBuf* in);

    //创建协议所需要的context
    void *alloc_proto_ctx(RpcContext* context);

    //移除创建的context
    void del_proto_ctx(void *context);

private:

    void reset(RpcContext *);

    ParseResult invoke_user_cb(RpcContext* context);
    void fill_reply_data(::google::protobuf::Message* req_msg, ::google::protobuf::Message* resp_msg, RpcContext *ctx);

};

}

#endif
