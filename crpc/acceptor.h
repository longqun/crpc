#ifndef CRPC_ACCEPTOR_H
#define CRPC_ACCEPTOR_H

#include <sys/select.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <errno.h>
#include <sys/types.h>
#include <strings.h>

#include "socket.h"
#include "poller.h"
#include "non_copy.h"
#include "common_header.h"

namespace crpc
{

class Acceptor  : NonCopy
{
public:
    int fd() const
    {
        return _acceptor_socket.fd();
    }

    explicit Acceptor();

    int init(const ServerOption& option);

    int accept()
    {
        struct sockaddr client_address;
        socklen_t client_addrlength = sizeof(client_address);
        return ::accept(_acceptor_socket.fd(), &client_address, &client_addrlength);
    }

private:
    ServerOption _option;
    Socket _acceptor_socket;
};

}


#endif
