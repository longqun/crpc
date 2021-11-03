#include "rpccontext.h"
#include "proto_rpc_controller.h"

namespace crpc
{

ProtoRpcController::ProtoRpcController(RpcContext* context):_context(context)
{
    //��ֹ���¼�
    if (context)
    {
        _context->add_ref();
    }
}

ProtoRpcController::~ProtoRpcController()
{
    //�������ζ��¼�
    if (_context)
    {
        _context->dec_ref();
    }

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
