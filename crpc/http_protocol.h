#ifndef HTTP_PROTOCOL_H
#define HTTP_PROTOCOL_H

#include <string>
#include <unordered_map>
#include "google/protobuf/service.h"
#include "google/protobuf/stubs/common.h"
#include "protocol.h"
#include "rpccontext.h"
#include "io_buf.h"

namespace crpc
{

//http协议解析
class HttpProtocol
{
public:
    HttpProtocol()
    {}

    static ParseResult proto_match(IoBuf* io_buf);

    ParseResult parse(IoBuf* io_buf);

    void process();

    void response(RpcContext* context, IoBuf* io_buf);

private:

    void fill_reply_data(::google::protobuf::Message* req_msg, ::google::protobuf::Message* resp_msg);


    //TODO 解决http keep-alive
    void reset();

    ProtoRpcController _controller;
    HttpRequestParser _req_parse;
};

}

#endif
