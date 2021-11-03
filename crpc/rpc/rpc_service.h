#ifndef CRPC_SERVICE_H
#define CRPC_SERVICE_H

#include <string>
#include <unordered_map>
#include <utility>
#include "non_copy.h"
#include "rpcmeta.pb.h"
#include "common_header.h"
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

    void register_rpc_service(::google::protobuf::Service* service);

    CallMessage get_call_msg(const std::string& service_name, const std::string& method);

    ::google::protobuf::Service* get_rpc_service(const std::string& service)
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
};

#define REG_RPC_SERVICE(service) RpcService::get_instance()->register_rpc_service(service)
#define GET_RPC_SERVICE(name) RpcService::get_instance()->get_rpc_service(name)

}

#endif
