#include "http_service.h"

namespace crpc{

const std::string str_http_method[] = {
    "GET",
    "POST",
    "HEAD",
    "PUT",
    "PATCH",
    "DELETE",
    "CONNECT",
    "OPTIONS",
    "TRACE"
};

void HttpService::register_service(HTTP_METHOD method, const std::string & url_path, const http_handle & handle)
{
    _tree[method].insert(url_path.c_str(), url_path.size(), handle);
}

void HttpService::unregister(HTTP_METHOD method, const std::string & url_path)
{
    _tree[method].remove(url_path.c_str(), url_path.size());
}

bool HttpService::find_http_handle(HTTP_METHOD method ,const std::string& url, http_handle& ret)
{
    if(_tree[method].find_prefix(url.c_str(), url.size(), ret))
        return true;

    return false;
}

HttpService *HttpService::get_instance()
{
    static HttpService service;
    return &service;
}



}

