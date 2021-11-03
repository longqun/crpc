#include <stddef.h>
#include "http_req_parse.h"
#include "http_protocol_context.h"
#include "crpc_log.h"
#include "http_protocol.h"

namespace crpc
{

ParseResult HttpProtocol::proto_match(IoBuf* io_buf)
{
    HttpRequestParser parser;
    return parser.check_http_proto(io_buf);
}

void *HttpProtocol::alloc_proto_ctx(RpcContext* context)
{
    HttpProtocolContext *http_context = new HttpProtocolContext(context);
    return http_context;
}

void HttpProtocol::del_proto_ctx(void * context)
{
    HttpProtocolContext *http_context = (HttpProtocolContext *)context;
    delete http_context;
}

ParseResult HttpProtocol::read_event(IoBuf* in, RpcContext* context)
{
    HttpProtocolContext *http_context = (HttpProtocolContext *)context->get_proto_context();
    return http_context->parse_http(in);
}

ParseResult HttpProtocol::write_event(RpcContext* context)
{
    HttpProtocolContext *http_context = (HttpProtocolContext *)context->get_proto_context();
    return http_context->write_event(context);
}


}

