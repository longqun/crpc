#include "rpc_protocol.h"
#include "crpc_log.h"

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

ParseResult RpcProtocol::parse(IoBuf * io_buf)
{
    char buff[CRPC_STR_LEN];
    RpcParseStatus pre_status = RpcParseStatus::INVALID_STATUS;
    while (pre_status != _header.status)
    {
        pre_status = _header.status;
        switch (_header.status)
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
                    _header.status = AFTER_CRPC;
                }
                break;
            case RpcParseStatus::AFTER_CRPC:
                if (io_buf->size() >= sizeof(size_t))
                {
                    io_buf->cutn(&_header.rpc_meta_size, sizeof(size_t));
                    _header.status = RpcParseStatus::META_SIZE;
                }
                break;
            case RpcParseStatus::META_SIZE:
                if (io_buf->size() >= _header.rpc_meta_size)
                {
                    //TODO parse failed
                    _header.rpc_meta.ParseFromString(io_buf->to_string(_header.rpc_meta_size));
                    _header.status = RpcParseStatus::META_DATA;
                }
                break;
            case RpcParseStatus::META_DATA:
                if (io_buf->size() >= _header.rpc_meta.data_size())
                {
                    _header.probuf_str = io_buf->to_string(_header.rpc_meta.data_size());
                }
                break;
        }
    }

    if (!_header.probuf_str.empty())
        return PARSE_SUCCESS;

    return NEED_NORE_DATA;
}

void RpcProtocol::fill_reply_data(::google::protobuf::Message* req_msg, ::google::protobuf::Message* resp_msg)
{
    std::unique_ptr<::google::protobuf::Message> req_guard(req_msg);
    std::unique_ptr<::google::protobuf::Message> resp_guard(resp_msg);

    //response header
    _response.proto_str = resp_msg->SerializeAsString();
    _response.response_len = _response.proto_str.size();
}

void RpcProtocol::invoke_user_cb()
{
    ProtoRpcController controller;
    ::google::protobuf::Service* service =  get_rpc_service(_header.rpc_meta.service_name());
    CallMessage msg =  RpcService::get_instance()->get_call_msg(_header.rpc_meta.service_name(), _header.rpc_meta.method_name());
    if(!msg.valid || !service)
    {
        _header.reset();
        return;
    }

    msg.req->ParseFromString(_header.probuf_str);
    auto done = ::google::protobuf::NewCallback(this, &RpcProtocol::fill_reply_data, msg.req, msg.resp);
    service->CallMethod(msg.md, &controller, msg.req, msg.resp, done);
    _header.reset();
}

void RpcProtocol::process()
{
    invoke_user_cb();
}

void RpcProtocol::response(RpcContext* context, IoBuf * io_buf)
{
    io_buf->append(&_response.response_len, sizeof(size_t));
    io_buf->append(_response.proto_str.c_str(), _response.proto_str.size());
    _response.reset();
}

}


