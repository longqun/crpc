#include "acceptor.h"
#include "crpc_log.h"

namespace crpc
{

Acceptor::Acceptor(EPoller* poller):_poller(poller), _acceptor_socket(socket(PF_INET, SOCK_STREAM, 0))
{
}

int Acceptor::init(const ServerOption& option)
{
    crpc_log("init acceptor addr %s port %d", option.addr().c_str(), option.port());

    _option = option;
    if (_acceptor_socket.set_non_blocking() < 0)
    {
        crpc_log("non blocking failed %d", errno);
        return -1;
    }

    int val = 1;
    if (setsockopt(_acceptor_socket.fd(), SOL_SOCKET, SO_REUSEPORT, &val, sizeof(val)) < 0)
    {
        crpc_log("set option failed %d", errno);
        return -1;
    }

    sockaddr_in address;
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(option.port());
    address.sin_addr.s_addr = inet_addr(option.addr().c_str());

    if (bind(_acceptor_socket.fd(), (sockaddr*)&address, sizeof(address)) < 0)
    {
        crpc_log("bind failed %d", errno);
        return -1;
    }

    if (listen(_acceptor_socket.fd(), 5) < 0)
    {
        crpc_log("listen failed %d", errno);
        return -1;
    }

    if (_poller->add_fd(_acceptor_socket.fd(), false) < 0)
    {
        crpc_log("add_fd failed %d", errno);
        return -1;
    }
    return 0;
}

}
