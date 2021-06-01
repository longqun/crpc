#ifndef CRPC_SOCKET_H
#define CRPC_SOCKET_H

#include <fcntl.h>
#include <unistd.h>

namespace crpc
{

class Socket
{
public:
    explicit Socket(int fd) :_fd(fd)
    {
    }

    int fd() const
    {
        return _fd;
    }

    void close()
    {
        ::close(_fd);
        _fd = -1;
    }

    int set_non_blocking();

private:
    int _fd;
};

}

#endif