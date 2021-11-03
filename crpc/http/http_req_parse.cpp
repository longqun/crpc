#include "http_common_header.h"
#include "http_protocol_context.h"
#include "http_req_parse.h"


namespace crpc
{

static std::vector<std::string> s_http_method{"GET", "HEAD", "POST", "PUT", "DELETE", "TRACE", "OPTIONS", "CONNECT"};

static std::string get_match_method(const std::string& line)
{
    for (size_t i = 0; i < s_http_method.size(); ++i)
    {
        if (strncmp(s_http_method[i].c_str(), line.c_str(), std::min(s_http_method[i].size(), line.size())) == 0)
            return s_http_method[i];
    }
    return std::string();
}


#define HTTP_METHOD_MAX_LEN (7)
ParseResult HttpRequestParser::check_http_proto(IoBuf* io_buf)
{
    if (io_buf->size() < HTTP_METHOD_MAX_LEN)
        return NEED_NORE_DATA;

    char buf[HTTP_METHOD_MAX_LEN + 1];
    io_buf->copyn(buf, sizeof(buf));
    buf[HTTP_METHOD_MAX_LEN] = '\0';

    if (get_match_method(buf).empty())
        return PARSE_FAILED;

    return PARSE_SUCCESS;
}

/*
static bool parse_head_line(const std::string& line, HttpHeader& header)
{
    size_t pos = line.find(":");
    if (pos == std::string::npos)
    {
        //not found
        return false;
    }
    //method
    std::string key = line.substr(0, pos);
    std::string value = line.substr(pos + 1);
    header.append_header(key, value);
    return true;
}
*/


static bool parse_fisrt_head_line(const std::string& line, http_req_line &req_line)
{
    std::string method = get_match_method(line);
    if (method.empty())
        return false;

    req_line.method_name = method;
    req_line.method = name_to_method(method);

    //TODO 字符串边界处理还要精细一点
    std::string key, value;
    //urlpath
    size_t url_start_pos = line.find(' ');
    size_t url_end_pos = line.find(' ', url_start_pos + 1);

    if (url_end_pos > url_start_pos && url_start_pos != std::string::npos && url_end_pos != std::string::npos)
    {
        value = line.substr(url_start_pos + 1, url_end_pos - url_start_pos - 1);
    }

    req_line.url = value;

    //http ver
    value = line.substr(url_end_pos + 1);

    req_line.http_ver = value;
    return true;
}


ParseResult HttpRequestParser::parse_req_line(http_req_line &req_line, IoBuf *io_buf)
{
    std::string line;
    //需要更多的数据才能解析
    if (!io_buf->cut_crlf(line))
        return NEED_NORE_DATA;

    return parse_fisrt_head_line(line, req_line) ? PARSE_SUCCESS : PARSE_FAILED;
}

bool parse_head_line_help(const std::string& line, HttpHeader& header)
{
    size_t pos = line.find(":");
    if (pos == std::string::npos)
    {
        //not found
        return false;
    }

    //method
    std::string key = line.substr(0, pos);
    std::string value = line.substr(pos + 1);
    header.append_header(key, value);
    return true;
}

//TODO 解析更加精细，能够知道是什么原因导致解析失败的
ParseResult HttpRequestParser::parse_header(HttpHeader &http_header, IoBuf *io_buf)
{
    std::string line;
    while (true)
    {
        //需要更多的数据才能解析
        if (!io_buf->cut_crlf(line))
            return NEED_NORE_DATA;

        //http头最后一行为空
        if (line.empty())
            return PARSE_SUCCESS;

        if (!parse_head_line_help(line, http_header))
            return PARSE_FAILED;

        http_header._header_size += line.size();
        if (http_header._header_size >= http_header._max_header_size)
            return PARSE_FAILED;

    }
    return PARSE_SUCCESS;
}

ParseResult HttpRequestParser::parse_body(HttpProtocolContext *context, IoBuf *io_buf, uint64_t content_length)
{
    if (io_buf->size() < content_length)
        return NEED_NORE_DATA;

    char *body = new char[content_length];
    context->set_req_body(body);

    io_buf->cutn(body, content_length);
    return PARSE_SUCCESS;
}


}

