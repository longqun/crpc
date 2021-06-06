#include <stddef.h>
#include "rpc_service.h"
#include "crpc_log.h"
#include "http_protocol.h"

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

ParseResult HttpProtocol::parse(IoBuf * io_buf)
{
    //header 
    return _req_parse.parse(io_buf);
}

ParseResult HttpProtocol::proto_match(IoBuf* io_buf)
{
    HttpRequestParser parser;
    return parser.check_http_proto(io_buf);
}

//TODO 这里不需要填充数据，看下怎么处理
void HttpProtocol::fill_reply_data(::google::protobuf::Message* req_msg, ::google::protobuf::Message* resp_msg)
{
    std::unique_ptr<::google::protobuf::Message> req_guard(req_msg);
    std::unique_ptr<::google::protobuf::Message> resp_guard(resp_msg);
}

void HttpProtocol::process()
{
    //set controller http req header
    HttpHeader& http_req = _req_parse.get_http_header();
    _controller.set_http_parser(&_req_parse);

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
        return;
    }
    auto done = ::google::protobuf::NewCallback(this, &HttpProtocol::fill_reply_data, msg.req, msg.resp);
    service->CallMethod(msg.md, &_controller, msg.req, msg.resp, done);
}

//TODO 处理发送10G的大文件，如果都写入到iobuf是否可行?
void HttpProtocol::response(RpcContext* context, IoBuf* io_buf)
{
    auto& ref_iobuf = _controller.get_write_io_buf();

    //write http response header
    append_http_response_header(io_buf, ref_iobuf.size());

    //write payload
    io_buf->append(&ref_iobuf);

    //TODO 支持keep-alive
    context->set_context_close();
    reset();
}

void HttpProtocol::reset()
{
    _req_parse.reset();
}

}

