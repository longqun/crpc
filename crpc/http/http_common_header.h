#ifndef HTTP_COMMON_HEADER_H_
#define HTTP_COMMON_HEADER_H_

#include <string>
#include "http_header.h"
#include "utills.h"

namespace crpc{

//http支持的方法
enum HTTP_METHOD{
    HTTP_GET = 0,
    HTTP_POST,
    HTTP_HEAD,
    HTTP_PUT,
    HTTP_PATCH,
    HTTP_DELETE,
    HTTP_CONNECT,
    HTTP_OPTIONS,
    HTTP_TRACE,
    HTTP_MAX,
    HTTP_INVALID
};

#define HTTP_CRLF               "\r\n"
#define HTTP_V1                 "HTTP/1.1"
#define HTTP_V0                 "HTTP/1.0"

//标准自带的header
#define HTTP_CONTENT_LENGTH             "Content-Length"
#define HTTP_CONECTION                  "Connection"
#define HTTP_CONTENT_TYPE               "Content-Type"
#define HTTP_ACCEPT_LANG                "Accept-Language"
#define HTTP_USER_AGENT                 "User-Agent"


#define HTTP_CONECTION_CLOSE                      "close"
#define HTTP_CONECTION_KEEP_ALIVE                 "keep-alive"


//将宏转成字符串
HTTP_METHOD name_to_method(const std::string& name);

//请求行
struct http_req_line {
    HTTP_METHOD method;
    std::string method_name;
    std::string url;
    std::string http_ver;

    std::string to_string()
    {
        return method_name + " " + url + " " + http_ver + HTTP_CRLF;
    }
};

//返回行
struct http_res_line {
    int http_code;
    std::string http_ver;

    http_res_line()
    {}

    http_res_line(int code):http_code(code), http_ver(HTTP_V1)
    {

    }

    std::string to_string()
    {
        return http_ver + " " + std::to_string(http_code) + " " + get_http_code_name(http_code) + HTTP_CRLF;
    }
};

//构建http返回值
std::string build_http_response(int http_code, const std::string &body);


};


#endif
