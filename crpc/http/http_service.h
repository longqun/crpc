#ifndef CRPC_HTTP_SERVICE_H_
#define CRPC_HTTP_SERVICE_H_

#include <string>
#include "http_protocol_context.h"
#include "http_common_header.h"
#include "common_header.h"
#include "trie.h"

namespace crpc
{

#define GET_HTTP_HANDLE(method, url, handle) HttpService::get_instance()->find_http_handle(method, url, handle)
#define REG_HTTP_HANDLE(method, url, handle) HttpService::get_instance()->register_service(method, url, handle);

class HttpService : NonCopy {
public:

    void register_service(HTTP_METHOD method, const std::string & url_path, const http_handle & handle);

    void unregister(HTTP_METHOD method, const std::string & url_path);

    bool find_http_handle(HTTP_METHOD method ,const std::string& url, http_handle& ret);

    static HttpService* get_instance();

protected:
    HttpService()
    {}

private:
    Trie<char, http_handle> _tree[HTTP_MAX];

};

}

#endif
