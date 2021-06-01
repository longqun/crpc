#ifndef CRPC_SERVICE_H
#define CRPC_SERVICE_H

#include <string>
#include <unordered_map>
#include <utility>
#include "non_copy.h"
#include "rpcmeta.pb.h"
#include "common_header.h"
#include "proto_rpc_controller.h"
#include "google/protobuf/service.h"
#include "google/protobuf/stubs/common.h"
#include "trie.h"

namespace crpc
{

struct service_info
{
    ::google::protobuf::Service* service;
    const ::google::protobuf::ServiceDescriptor* sd;
    std::unordered_map<std::string, const ::google::protobuf::MethodDescriptor*> mds;
};

struct CallMessage
{
    //ÊÇ·ñok
    bool valid;
    const ::google::protobuf::MethodDescriptor* md;
    google::protobuf::Message* req;
    google::protobuf::Message* resp;

    CallMessage(bool valid_, const ::google::protobuf::MethodDescriptor* md_ = NULL,
                    google::protobuf::Message* req_ = NULL, google::protobuf::Message* resp_ = NULL)
                    :valid(valid_), md(md_), req(req_), resp(resp_)
    {};
};

class RpcService : NonCopy
{
public:
    static RpcService* get_instance();

    void add_service(::google::protobuf::Service* service);

    void add_http_service(::google::protobuf::Service* service, const std::string& url, const std::string& method);

    CallMessage get_call_msg(const std::string& service_name, const std::string& method);

    ::google::protobuf::Service* get_service(const std::string& service)
    {
        if (_services.find(service) == _services.end())
            return NULL;
        return _services[service].service;
    }

    std::pair<std::string, std::string> get_service_method_pair(const std::string& url);

private:
    RpcService();
    static void create_instance();
    std::unordered_map<std::string, service_info> _services;

    //http url -> service_name & method
    std::unordered_map<std::string, std::pair<std::string, std::string>> _url_to_service_map;
    Trie<char, std::string> _trie;

};

#define add_rpc_service(service) RpcService::get_instance()->add_service(service)
#define add_rpc_http_service(service, service_name, method) RpcService::get_instance()->add_http_service(service, service_name, method)
#define get_rpc_service(name) RpcService::get_instance()->get_service(name)
//#define get_service_method_pair(url) RpcService::get_instance()->get_service_method_pair(url)

}

#endif
