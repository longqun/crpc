#include "rpccontext.h"
#include "proto_rpc_controller.h"

namespace crpc
{

ProtoRpcController::ProtoRpcController(RpcContext* context, HttpRequestParser* req):_context(context)
{
    if (req)
        _http_req_parse = std::move(*req);
    _context->add_ref();
}

ProtoRpcController::~ProtoRpcController()
{
    _context->dec_ref();
}

EventLoop* ProtoRpcController::get_runing_loop()
{
    return _context->get_context_loop();
}

RpcContext* ProtoRpcController::get_context()
{
    return _context;
}

}