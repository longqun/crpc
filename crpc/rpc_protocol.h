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
class RpcProtocol
{
public:
    RpcProtocol()
    {}

    static ParseResult proto_match(IoBuf* io_buf);

    ParseResult parse(IoBuf* io_buf);

    void process();

    void response(RpcContext* context, IoBuf* io_buf);

private:
    void invoke_user_cb();
    void fill_reply_data(::google::protobuf::Message* req_msg, ::google::protobuf::Message* resp_msg);
    rpc_header _header;
    rpc_response _response;
};

}

#endif
