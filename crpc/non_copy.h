#ifndef CRPC_NONCOPY_H
#define CRPC_NONCOPY_H

namespace crpc
{

class NonCopy {
protected:
    NonCopy(const NonCopy&) = delete;
    NonCopy& operator=(const NonCopy&) = delete;

    NonCopy() = default;
    ~NonCopy() = default;
};

}

#endif