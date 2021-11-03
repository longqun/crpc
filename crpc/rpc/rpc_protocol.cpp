#include "rpc_protocol.h"
#include "crpc_log.h"
#include "rpc_closure.h"

namespace crpc
{

ParseResult RpcProtocol::proto_match(IoBuf* io_buf)
{
    //check first 4 bytes crpc
    if (io_buf->size() < CRPC_STR_LEN)
        return NEED_NORE_DATA;

    char buf[CRPC_STR_LEN];
    io_buf->copyn(buf, sizeof(buf));
    if (strncmp(buf, CRPC_STR, CRPC_STR_LEN) == 0)
        return PARSE_SUCCESS;

    return PARSE_FAILED;
}

void *RpcProtocol::alloc_proto_ctx(RpcContext* context)
{
    RpcProtocolContext *ctx = new RpcProtocolContext;
    ctx->_header.reset();
    ctx->_response.reset();
    return ctx;
}

     //读事件
 ParseResult RpcProtocol::read_event(IoBuf* in, RpcContext* context)
{
    RpcProtocolContext *ctx = (RpcProtocolContext *)context->get_proto_context();
    rpc_header &header = ctx->_header;

    //means last call not finished yet
    if (!header.probuf_str.empty())
        return NEED_NORE_DATA;

    char buff[CRPC_STR_LEN];
    RpcParseStatus pre_status = RpcParseStatus::INVALID_STATUS;
    while (pre_status != header.status)
    {
        pre_status = header.status;
        switch (header.status)
        {
            case RpcParseStatus::INVALID_STATUS:
                break;
            case RpcParseStatus::INIT:
                // check crpc str
                if (in->size() >= sizeof(buff))
                {
                    in->cutn(buff, sizeof(buff));
                    if (strncmp(buff, CRPC_STR, CRPC_STR_LEN) != 0)
                    {
                        crpc_log("proto not match !!! %.*s", CRPC_STR_LEN, buff);
                        return PARSE_FAILED;
                    }
                    header.status = AFTER_CRPC;
                }
                break;
            case RpcParseStatus::AFTER_CRPC:
                if (in->size() >= sizeof(size_t))
                {
                    in->cutn(&header.rpc_meta_size, sizeof(size_t));
                    header.status = RpcParseStatus::META_SIZE;
                }
                break;
            case RpcParseStatus::META_SIZE:
                if (in->size() >= header.rpc_meta_size)
                {
                    //TODO parse failed
                    header.rpc_meta.ParseFromString(in->to_string(header.rpc_meta_size));
                    header.status = RpcParseStatus::META_DATA;
                }
                break;
            case RpcParseStatus::META_DATA:
                if (in->size() >= header.rpc_meta.data_size())
                {
                    header.probuf_str = in->to_string(header.rpc_meta.data_size());
                }
                break;
        }
    }

    if (!header.probuf_str.empty())
    {
        //disable read event
        return invoke_user_cb(context);
    }

    return NEED_NORE_DATA;
}

//针对rpc来说目前实现都是一次性写完。后续有大数据要传输再实现异步传输
ParseResult RpcProtocol::write_event(RpcContext* context)
{
    return PARSE_SUCCESS;
}

void RpcProtocol::del_proto_ctx(void *context)
{
    RpcProtocolContext *ctx = (RpcProtocolContext *)context;
    delete ctx;
}

void RpcProtocol::fill_reply_data(::google::protobuf::Message* req_msg, ::google::protobuf::Message* resp_msg, RpcContext *context)
{
    RpcProtocolContext *ctx = (RpcProtocolContext *)context->get_proto_context();
    std::unique_ptr<::google::protobuf::Message> req_guard(req_msg);
    std::unique_ptr<::google::protobuf::Message> resp_guard(resp_msg);

    //response header
    ctx->_response.proto_str = resp_msg->SerializeAsString();
    ctx->_response.response_len =  ctx->_response.proto_str.size();

    context->context_write((const char *)&ctx->_response.response_len, sizeof(size_t));
    context->context_write(ctx->_response.proto_str.c_str(), ctx->_response.proto_str.size());

    context->enable_read();
    reset(context);
}

ParseResult RpcProtocol::invoke_user_cb(RpcContext* context)
{
    RpcProtocolContext *ctx = (RpcProtocolContext *)context->get_proto_context();
    ProtoRpcController* controller = new ProtoRpcController(context);
    ::google::protobuf::Service* service =  GET_RPC_SERVICE(ctx->_header.rpc_meta.service_name());
    CallMessage msg =  RpcService::get_instance()->get_call_msg(ctx->_header.rpc_meta.service_name(), ctx->_header.rpc_meta.method_name());
    if(!msg.valid || !service)
    {
        reset(context);
        return PARSE_FAILED;
    }

    //在调用函数的时候禁止读
    context->disable_read();

    //TODO handle parse failed!
    msg.req->ParseFromString(ctx->_header.probuf_str);
    auto done = new CRpcClosure(controller, std::bind(&RpcProtocol::fill_reply_data, this, msg.req, msg.resp, context));
    service->CallMethod(msg.md, controller, msg.req, msg.resp, done);

    //在 controller 析构之后就会允许读，如果不允许读则表示controller还未析构,需要等待结束
    if (!context->has_read_interest())
    {
        return NEED_NORE_DATA;
    }

    return PARSE_SUCCESS;
}

void RpcProtocol::reset(RpcContext* context)
{
    RpcProtocolContext *ctx = (RpcProtocolContext *)context->get_proto_context();
    ctx->_header.reset();
    ctx->_response.reset();
}

}


