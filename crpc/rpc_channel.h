#ifndef RPC_CHANNEL_H
#define RPC_CHANNEL_H

#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <vector>
#include <netinet/in.h>

#include "rpcmeta.pb.h"
#include "common_header.h"
#include "google/protobuf/service.h"
#include "google/protobuf/stubs/common.h"
#include "socket.h"

namespace crpc
{


class RpcChannel : public ::google::protobuf::RpcChannel 
{
public:

    RpcChannel():_socket(socket(AF_INET, SOCK_STREAM, 0))
    {}

    int init(const std::string& addr, int port)
    {
        _option.set_option(port, addr);

        struct sockaddr_in  servaddr;
        memset(&servaddr, 0, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(port);
        if(inet_pton(AF_INET, addr.c_str(), &servaddr.sin_addr) <= 0)
        {
            return -1;
        }

        if(connect(_socket.fd(), (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
        {
            return -1;
        }
        return 0;
    }

    virtual void CallMethod(const ::google::protobuf::MethodDescriptor* method,
            ::google::protobuf::RpcController* /* controller */,
            const ::google::protobuf::Message* request,
            ::google::protobuf::Message* response,
            ::google::protobuf::Closure*);

private:
    ServerOption _option;
    Socket _socket;
};


}
#endif
