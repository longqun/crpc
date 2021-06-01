#include "socket.h"

namespace crpc {

int Socket::set_non_blocking()
{
    int option = fcntl(_fd, F_GETFL);
    option |= O_NONBLOCK;
    return fcntl(_fd, F_SETFL, option);
}

}
