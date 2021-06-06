#ifndef CRPC_PROTO_RPC_CONTROLLER_H
#define CRPC_PROTO_RPC_CONTROLLER_H

#include <string>
#include "google/protobuf/service.h"
#include "google/protobuf/stubs/common.h"
#include "rpccontext.h"
#include "io_buf.h"
#include "http_req_parse.h"

namespace crpc{
class RpcContext;
class EventLoop;

//fake controller
class ProtoRpcController : public ::google::protobuf::RpcController {
    public:
      ProtoRpcController(RpcContext* context, HttpRequestParser* req = NULL);

      ~ProtoRpcController();


      virtual void Reset() { };
      virtual bool Failed() const { return false; };
      virtual std::string ErrorText() const { return ""; };
      virtual void StartCancel() { };
      virtual void SetFailed(const std::string& /* reason */) { };
      virtual bool IsCanceled() const { return false; };
      virtual void NotifyOnCancel(::google::protobuf::Closure* /* callback */) { };

    IoBuf& get_write_io_buf()
    {
        return _write_io;
    }

    HttpRequestParser* get_http_parser()
    {
        return &_http_req_parse;
    }

    EventLoop* get_runing_loop();

    RpcContext* get_context();


    private:
        RpcContext* _context;
        HttpRequestParser _http_req_parse;
        //针对http这种无法直接写入到返回中的，就写入到这里面
        IoBuf _write_io;
};

}

#endif
