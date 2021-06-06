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

static bool parse_fisrt_head_line(const std::string& line, HttpHeader& header)
{
    std::string method = get_match_method(line);
    if (method.empty())
        return false;

    header.append_header(HTTP_METHOD, method);

    //TODO 字符串边界处理还要精细一点
    std::string key, value;
    //urlpath
    size_t url_start_pos = line.find(' ');
    size_t url_end_pos = line.find(' ', url_start_pos + 1);

    if (url_end_pos > url_start_pos && url_start_pos != std::string::npos && url_end_pos != std::string::npos)
    {
        value = line.substr(url_start_pos + 1, url_end_pos - url_start_pos - 1);
        header.append_header(HTTP_URL_PATH, value);
    }

    //http ver
    value = line.substr(url_end_pos + 1);
    header.append_header(HTTP_VERSION, value);
    return true;
}

void HttpRequestParser::dump_header()
{
    _req_header.dump_http_header();
}

#define HTTP_METHOD_MAX_LEN (7)
ParseResult HttpRequestParser::check_http_proto(IoBuf* io_buf)
{
    //
    if (io_buf->size() < HTTP_METHOD_MAX_LEN)
        return NEED_NORE_DATA;

    char buf[HTTP_METHOD_MAX_LEN + 1];
    io_buf->copyn(buf, sizeof(buf));
    buf[HTTP_METHOD_MAX_LEN] = '\0';

    if (get_match_method(buf).empty())
        return PARSE_FAILED;

    return PARSE_SUCCESS;
}

ParseResult HttpRequestParser::http_parse_header(IoBuf* io_buf)
{
    std::string line;
    while (_req_header_status != HTTP_REQ_HEADER_END)
    {
        //需要更多的数据才能解析
        if (!io_buf->cut_crlf(line))
            return NEED_NORE_DATA;

        switch (_req_header_status)
        {
            case HTTP_REQ_HEADER_FIRST_LINE:
                if(parse_fisrt_head_line(line, _req_header))
                    _req_header_status = HTTP_REQ_HEADER_OTHER;
                else
                    return PARSE_FAILED;
            break;
            case HTTP_REQ_HEADER_OTHER:
                if (line.empty())
                {
                    _req_header_status = HTTP_REQ_HEADER_END;
                }
                else if (!parse_head_line(line, _req_header))
                {
                    return PARSE_FAILED;
                }
            break;
            case HTTP_REQ_HEADER_END:
                crpc_log("parse finished");;
            break;
        }
    }

    return PARSE_SUCCESS;
}

ParseResult HttpRequestParser::http_parse_body(IoBuf* io_buf)
{
    int64_t content_length = _req_header.get_content_length();
    if (!content_length)
        return PARSE_SUCCESS;

    if (io_buf->size() < content_length)
        return NEED_NORE_DATA;

    if (_body_buff)
        delete [] _body_buff;

    _body_size = content_length;
    _body_buff = new unsigned char[content_length];
    io_buf->cutn(_body_buff, _body_size);
    return PARSE_SUCCESS;
}

ParseResult HttpRequestParser::parse(IoBuf* io_buf)
{
    ParseResult result = PARSE_SUCCESS;
    while (_req_parse_status != HTTP_REQ_PARSE_OK)
    {
        switch (_req_parse_status)
        {
            case HTTP_REQ_PARSE_HEADER:
                result = http_parse_header(io_buf);
                if (result != PARSE_SUCCESS)
                    return result;
                _req_parse_status = HTTP_REQ_PARSE_OK;
                break;
            case HTTP_REQ_PARSE_BODY:
                //根据http头中的字段 Content-Length
                result = http_parse_body(io_buf);
                if (result != PARSE_SUCCESS)
                    return result;
                _req_parse_status = HTTP_REQ_PARSE_OK;
                break;
            case HTTP_REQ_PARSE_OK:
                crpc_log("http_header parse finished");
                break;
        }
    }
    return result;
}

}

