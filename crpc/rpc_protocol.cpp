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

void *RpcProtocol::alloc_proto_ctx()
{
    RpcProtocolContext *ctx = new RpcProtocolContext;
    ctx->_header.reset();
    ctx->_response.reset();
    return ctx;
}

void RpcProtocol::del_proto_ctx(void *context)
{
    RpcProtocolContext *ctx = (RpcProtocolContext *)context;
    delete ctx;
}

ParseResult RpcProtocol::proto_parse(IoBuf* io_buf, RpcContext* context)
{
    RpcProtocolContext *ctx = (RpcProtocolContext *)context->get_proto_context();
    rpc_header &header = ctx->_header;

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
                if (io_buf->size() >= sizeof(buff))
                {
                    io_buf->cutn(buff, sizeof(buff));
                    if (strncmp(buff, CRPC_STR, CRPC_STR_LEN) != 0)
                    {
                        crpc_log("proto not match !!! %.*s", CRPC_STR_LEN, buff);
                        return PARSE_FAILED;
                    }
                    header.status = AFTER_CRPC;
                }
                break;
            case RpcParseStatus::AFTER_CRPC:
                if (io_buf->size() >= sizeof(size_t))
                {
                    io_buf->cutn(&header.rpc_meta_size, sizeof(size_t));
                    header.status = RpcParseStatus::META_SIZE;
                }
                break;
            case RpcParseStatus::META_SIZE:
                if (io_buf->size() >= header.rpc_meta_size)
                {
                    //TODO parse failed
                    header.rpc_meta.ParseFromString(io_buf->to_string(header.rpc_meta_size));
                    header.status = RpcParseStatus::META_DATA;
                }
                break;
            case RpcParseStatus::META_DATA:
                if (io_buf->size() >= header.rpc_meta.data_size())
                {
                    header.probuf_str = io_buf->to_string(header.rpc_meta.data_size());
                }
                break;
        }
    }

    if (!header.probuf_str.empty())
        return PARSE_SUCCESS;

    return NEED_NORE_DATA;
}

void RpcProtocol::fill_reply_data(::google::protobuf::Message* req_msg, ::google::protobuf::Message* resp_msg, 
RpcContext *context)
{
    RpcProtocolContext *ctx = (RpcProtocolContext *)context->get_proto_context();
    std::unique_ptr<::google::protobuf::Message> req_guard(req_msg);
    std::unique_ptr<::google::protobuf::Message> resp_guard(resp_msg);

    //response header
    ctx->_response.proto_str = resp_msg->SerializeAsString();
    ctx->_response.response_len =  ctx->_response.proto_str.size();
}

void RpcProtocol::invoke_user_cb(RpcContext* context)
{
    RpcProtocolContext *ctx = (RpcProtocolContext *)context->get_proto_context();
    ProtoRpcController* controller = new ProtoRpcController(context);
    ::google::protobuf::Service* service =  get_rpc_service(ctx->_header.rpc_meta.service_name());
    CallMessage msg =  RpcService::get_instance()->get_call_msg(ctx->_header.rpc_meta.service_name(), ctx->_header.rpc_meta.method_name());
    if(!msg.valid || !service)
    {
        ctx->_header.reset();
        return;
    }

    msg.req->ParseFromString(ctx->_header.probuf_str);
    auto done = new CRpcClosure(controller, std::bind(&RpcProtocol::fill_reply_data, this, msg.req, msg.resp, context));
    service->CallMethod(msg.md, controller, msg.req, msg.resp, done);
    ctx->_header.reset();
}

void RpcProtocol::proto_process(RpcContext* context)
{
    invoke_user_cb(context);
}

void RpcProtocol::proto_response(ProtoRpcController* controller ,RpcContext* context, IoBuf * io_buf)
{
    RpcProtocolContext *ctx = (RpcProtocolContext *)context->get_proto_context();
    io_buf->append(&ctx->_response.response_len, sizeof(size_t));
    io_buf->append(ctx->_response.proto_str.c_str(), ctx->_response.proto_str.size());
    ctx->_response.reset();
}

void RpcProtocol::proto_finished(RpcContext* context)
{

}


}


