#ifndef CRPC_COMMON_HEADER_H
#define CRPC_COMMON_HEADER_H

#include <string>
#include <unordered_map>
#include "google/protobuf/service.h"
#include "google/protobuf/stubs/common.h"
#include <google/protobuf/descriptor.h>


namespace crpc
{
class ServerOption
{
public:
    ServerOption(int port, const std::string& addr):_port(port), _addr(addr)
    {}


    ServerOption()
    {}

    void set_option(int port, const std::string& addr)
    {
        _port = port;
        _addr = addr;
    }

    int port() const
    {
        return _port;
    }

    const std::string& addr() const
    {
        return _addr;
    }

private:
    int _port;
    std::string _addr;
};

}

#endif
