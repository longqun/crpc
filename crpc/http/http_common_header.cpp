#include <unordered_map>
#include "http_common_header.h"


namespace crpc
{

static std::unordered_map<std::string, HTTP_METHOD> s_method_name_map  {
    {"GET", HTTP_GET},
    {"POST", HTTP_POST},
    {"HEAD", HTTP_HEAD,},
    {"PUT", HTTP_PUT},
    {"PATCH", HTTP_PATCH},
    {"DELETE", HTTP_DELETE},
    {"CONNECT", HTTP_CONNECT},
    {"OPTIONS", HTTP_OPTIONS},
    {"TRACE", HTTP_TRACE}
};

HTTP_METHOD name_to_method(const std::string &name)
{
    if (s_method_name_map.find(name) != s_method_name_map.end())
        return s_method_name_map[name];
    return HTTP_INVALID;
}

std::string build_http_response(int http_code, const std::string &body)
{
    std::string ret;

    http_res_line res_line;
    res_line.http_ver = HTTP_V1;
    res_line.http_code = http_code;
    ret += res_line.to_string();

    HttpHeader header;
    header.append_header(HTTP_CONECTION, HTTP_CONECTION_CLOSE);
    header.append_header(HTTP_CONTENT_LENGTH, std::to_string(body.size()));

    ret += header.to_string();
    return ret + body;
}
};