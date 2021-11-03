#ifndef CRPC_HTTP_REQ_PARSE_H
#define CRPC_HTTP_REQ_PARSE_H

#include <unordered_map>
#include <string>
#include "http_service.h"
#include "io_buf.h"
#include "protocol.h"
#include "http_header.h"
#include "crpc_log.h"
#include "common_header.h"

namespace crpc{


class HttpProtocolContext;

//状态机
enum HTTP_REQ_PARSE_STATUS
{
    HTTP_REQ_PARSE_INIT,
    HTTP_REQ_PARSE_REQ_LINE,
    HTTP_REQ_PARSE_HEADER,
    HTTP_REQ_PARSE_OK,
};

enum HTTP_PARSE_STATUS {
    HTTP_PARSE_REQ_LINE_FAILED,
    HTTP_PARSE_REQ_HEADER_FAILED,
    HTTP_PARSE_NEED_DATA,       //需要更多的数据
    HTTP_PARSE_OK,              //解析ok
    HTTP_PARSE_FAILED           //解析失败
};


//存储http头解析的东西
class HttpRequestParser
{
public:
    HttpRequestParser()
    {}

    ~HttpRequestParser()
    {
    }

    ParseResult parse_req_line(http_req_line &req_line, IoBuf *io_buf);

    ParseResult parse_header(HttpHeader &http_header, IoBuf *io_buf);

    ParseResult parse_body(HttpProtocolContext *context, IoBuf *io_buf, uint64_t content_length);


    ParseResult check_http_proto(IoBuf *io_buf);

};

}
#endif
