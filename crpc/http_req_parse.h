#ifndef CRPC_HTTP_REQ_PARSE_H
#define CRPC_HTTP_REQ_PARSE_H

#include <unordered_map>
#include <string>
#include "protocol.h"
#include "http_header.h"
#include "crpc_log.h"

namespace crpc{


//状态机
enum HTTP_REQ_PARSE_STATUS
{
    HTTP_REQ_PARSE_HEADER,
    HTTP_REQ_PARSE_BODY,
    HTTP_REQ_PARSE_OK,
};

enum HTTP_REQ_HEADER_STATUS
{
    HTTP_REQ_HEADER_FIRST_LINE,
    HTTP_REQ_HEADER_OTHER,
    HTTP_REQ_HEADER_END,
};

//存储http头解析的东西
class HttpRequestParser
{
public:
    HttpRequestParser():_req_header_status(HTTP_REQ_HEADER_FIRST_LINE), 
    _req_parse_status(HTTP_REQ_PARSE_HEADER),_body_buff(NULL), _body_size(0)
    {}

    ~HttpRequestParser()
    {
        reset();
    }

    HttpRequestParser& operator= (HttpRequestParser &&req)
    {
        _req_header_status = req._req_header_status;
        _req_parse_status = req._req_parse_status;
        _req_header = std::move(req._req_header);
        req._req_header.reset();

        _body_buff = req._body_buff;
        _body_size = req._body_size;

        req._body_buff = NULL;
        req._body_size = 0;
        return *this;
    }

    HttpHeader& get_http_header()
    {
        return _req_header;
    }

    unsigned char* get_body()
    {
        return _body_buff;
    }

    int64_t body_size() const
    {
        return _body_size;
    }

    void reset()
    {
        _req_header_status = HTTP_REQ_HEADER_FIRST_LINE;
        _req_parse_status = HTTP_REQ_PARSE_HEADER;
        _req_header.reset();
        delete _body_buff;
        _body_buff = NULL;
        _body_size = 0;
    }

    ParseResult http_parse_header(IoBuf* io_buf);

    ParseResult http_parse_body(IoBuf* io_buf);

    ParseResult parse(IoBuf* io_buf);

    void dump_header();

    //检测是否是http 请求
    ParseResult check_http_proto(IoBuf* io_buf);

private:
    HTTP_REQ_HEADER_STATUS _req_header_status;
    HTTP_REQ_PARSE_STATUS _req_parse_status;
    HttpHeader _req_header;

    //TODO change to io_buf
    unsigned char* _body_buff;
    int64_t _body_size;
};

}
#endif
