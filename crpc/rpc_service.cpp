#include "rpc_service.h"

namespace crpc
{

static pthread_once_t once = PTHREAD_ONCE_INIT;
static RpcService* s_instance;

RpcService* RpcService::get_instance()
{
    pthread_once(&once, RpcService::create_instance);
    return s_instance;
}

void RpcService::create_instance()
{
    s_instance = new RpcService();
}

RpcService::RpcService()
{

}

CallMessage RpcService::get_call_msg(const std::string& service_name, const std::string& method)
{
    if (_services.find(service_name) == _services.end())
    {
        return CallMessage(false);
    }
    auto service = _services[service_name].service;
    auto md = _services[service_name].mds[method];
    return CallMessage(true, md, service->GetRequestPrototype(md).New(), service->GetResponsePrototype(md).New());
}

void RpcService::add_service(::google::protobuf::Service * service)
{
    service_info ser_info;
    ser_info.service = service;
    ser_info.sd = service->GetDescriptor();
    for (int i = 0; i < ser_info.sd->method_count(); ++i)
    {
        ser_info.mds[ser_info.sd->method(i)->name()] = ser_info.sd->method(i);
    }

    _services[ser_info.sd->name()] = ser_info;
}

std::pair<std::string, std::string> RpcService::get_service_method_pair(const std::string& url)
{
    std::string fit_url;
    if (!_trie.find_prefix(url.c_str(), url.size(), fit_url))
    {
        return std::pair<std::string, std::string>();
    }

    return _url_to_service_map[fit_url];
}

void RpcService::add_http_service(::google::protobuf::Service* service, const std::string& url, const std::string& 
method)
{
    std::string service_name = service->GetDescriptor()->name();
    add_service(service);
    _url_to_service_map[url] = make_pair(service_name, method);
    _trie.insert(url.c_str(), url.size(), url);
}

}
