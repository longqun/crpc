#ifndef CRPC_CLOSURE_H
#define CRPC_CLOSURE_H

#include <functional>
#include "google/protobuf/service.h"
#include "google/protobuf/stubs/common.h"

typedef std::function<void ()> closure_cb;

namespace crpc
{
class ProtoRpcController;

class CRpcClosure : public google::protobuf::Closure
{
public:
    CRpcClosure(ProtoRpcController* con, const closure_cb& cb);

    ~CRpcClosure();

    void Run() override;

    void set_cb(const closure_cb& cb)
    {
        _cb = cb;
    }

private:
    void run_internal();
    ProtoRpcController *_controller;
    closure_cb _cb;
};

}

#endif