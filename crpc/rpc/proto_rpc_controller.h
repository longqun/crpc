#ifndef CRPC_PROTO_RPC_CONTROLLER_H
#define CRPC_PROTO_RPC_CONTROLLER_H

#include <string>
#include "google/protobuf/service.h"
#include "google/protobuf/stubs/common.h"
#include "rpccontext.h"
#include "io_buf.h"

namespace crpc{
class RpcContext;
class EventLoop;

//fake controller
class ProtoRpcController : public ::google::protobuf::RpcController {
    public:
      ProtoRpcController(RpcContext* context);

      ~ProtoRpcController();

      virtual void Reset() { };
      virtual bool Failed() const { return false; };
      virtual std::string ErrorText() const { return ""; };
      virtual void StartCancel() { };
      virtual void SetFailed(const std::string& /* reason */) { };
      virtual bool IsCanceled() const { return false; };
      virtual void NotifyOnCancel(::google::protobuf::Closure* /* callback */) { };


    EventLoop* get_runing_loop();

    RpcContext* get_context();

    private:
        RpcContext* _context;
};

}

#endif
