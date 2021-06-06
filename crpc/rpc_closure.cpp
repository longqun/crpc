#include "rpc_closure.h"
#include "proto_rpc_controller.h"

namespace crpc
{

CRpcClosure::CRpcClosure(ProtoRpcController* con, const closure_cb& cb):_controller(con), _cb(cb)
{
    
}

CRpcClosure::~CRpcClosure()
{
    delete _controller;
}

//run in loop thread
void CRpcClosure::run_internal()
{
    //fill data
    if (_cb)
        _cb();
    _controller->get_context()->trigger_response(_controller);
    delete this;
}

void CRpcClosure::Run()
{
    _controller->get_runing_loop()->run_in_loop(std::bind(&CRpcClosure::run_internal, this));
}

};