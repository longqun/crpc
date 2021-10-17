#include <stddef.h>
#include "rpc_service.h"
#include "crpc_log.h"
#include "http_protocol.h"
#include "rpc_closure.h"

namespace crpc
{

#define HTTP_RESPONSE_HTTP_TYPE         "HTTP/1.1 200 OK\r\n"
#define HTTP_RESPONSE_SERVER            "Server: crpc\r\n"
#define HTTP_RESPONSE_CONTENT_TYPE      "Content-Type: text/html; charset=utf-8\r\n"
#define HTTP_RESPONSE_CONNETION         "Connection: close\r\n"
#define HTTP_RESPONSE_CONNTENT_LEN      "Content-Length: %d\r\n"
#define HTTP_RESPONSE_CRLF              "\r\n"
void append_http_response_header(IoBuf* io_buf, int64_t payload_size)
{
    io_buf->append(HTTP_RESPONSE_HTTP_TYPE, strlen(HTTP_RESPONSE_HTTP_TYPE));
    io_buf->append(HTTP_RESPONSE_SERVER, strlen(HTTP_RESPONSE_SERVER));
    io_buf->append(HTTP_RESPONSE_CONTENT_TYPE, strlen(HTTP_RESPONSE_CONTENT_TYPE));
    io_buf->append(HTTP_RESPONSE_CONNETION, strlen(HTTP_RESPONSE_CONNETION));

    if (payload_size)
    {
        char buf[4096];
        int len = snprintf(buf, sizeof(buf),HTTP_RESPONSE_CONNTENT_LEN, payload_size);
        io_buf->append(buf, len);
    }

    io_buf->append(HTTP_RESPONSE_CRLF, strlen(HTTP_RESPONSE_CRLF));
}

ParseResult HttpProtocol::proto_parse(IoBuf * io_buf, RpcContext *context)
{
    HttpRequestParser *req_parse = (HttpRequestParser *)context->get_proto_context();
    //context复用？
    return req_parse->parse(io_buf);
}

ParseResult HttpProtocol::proto_match(IoBuf* io_buf)
{
    HttpRequestParser parser;
    return parser.check_http_proto(io_buf);
}

void *HttpProtocol::alloc_proto_ctx()
{
    HttpRequestParser *req_parse = new HttpRequestParser();
    req_parse->reset();
    return req_parse;
}

void HttpProtocol::del_proto_ctx(void * context)
{
    HttpRequestParser *req_parse = (HttpRequestParser *)context;
    delete req_parse;
}

//TODO 这里不需要填充数据，看下怎么处理
void HttpProtocol::fill_reply_data(::google::protobuf::Message* req_msg, ::google::protobuf::Message* resp_msg)
{
    std::unique_ptr<::google::protobuf::Message> req_guard(req_msg);
    std::unique_ptr<::google::protobuf::Message> resp_guard(resp_msg);
}

void HttpProtocol::proto_process(RpcContext* context)
{
    //set controller http req header
    HttpRequestParser *req_parse = (HttpRequestParser *)context->get_proto_context();
    HttpHeader& http_req = req_parse->get_http_header();
    //invoke by url
    std::pair<std::string, std::string> pair = RpcService::get_instance()->get_service_method_pair(http_req.get_header(std::string(HTTP_URL_PATH)));
    //now we get service & method just call it
    ::google::protobuf::Service* service =  get_rpc_service(pair.first);
    CallMessage msg =  RpcService::get_instance()->get_call_msg(pair.first, pair.second);

    //log bad request
    if (!msg.valid || !service)
    {
        crpc_log("bad request!");
        http_req.dump_http_header();
        //no valid function close it!!
        context->set_context_close();
        return;
    }

    ProtoRpcController* controller = new ProtoRpcController(context, req_parse);
    auto done = new  CRpcClosure(controller, std::bind(&HttpProtocol::fill_reply_data, this, msg.req, msg.resp));
    service->CallMethod(msg.md, controller, msg.req, msg.resp, done);

    proto_finished(context);
}

//TODO 处理发送10G的大文件，如果都写入到iobuf是否可行?
void HttpProtocol::proto_response(ProtoRpcController* controller, RpcContext* context, IoBuf* io_buf)
{
    auto& ref_iobuf = controller->get_write_io_buf();

    //write http response header
    append_http_response_header(io_buf, ref_iobuf.size());

    //write payload
    io_buf->append(&ref_iobuf);

    //TODO 支持keep-alive
    context->set_context_close();
}

void HttpProtocol::proto_finished(RpcContext* context)
{
    HttpRequestParser *req_parse = (HttpRequestParser *)context->get_proto_context();
    req_parse->reset();
}

}

