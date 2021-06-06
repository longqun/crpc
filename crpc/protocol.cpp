#include "protocol.h"
#include "rpccontext.h"
#include "rpc_protocol.h"
#include "http_protocol.h"

namespace crpc
{

//------default_proto--------

template <typename T>
ParseResult default_proto_match(IoBuf* io_buf)
{
    return T::proto_match(io_buf);
}

template <typename T>
void *proto_new()
{
    return (void *)new T();
}

template <typename T>
ParseResult default_proto_parse(IoBuf* io_buf, void* user_data)
{
    T* call_proto = (T*)user_data;
    return call_proto->parse(io_buf);
}

template <typename T>
void default_proto_process(void* user_data)
{
    T* call_proto = (T*)user_data;
    call_proto->process();
}

template <typename T>
void default_proto_response(RpcContext* context, IoBuf* io_buf, void* user_data)
{
    T* call_proto = (T*)user_data;
    call_proto->response(context, io_buf);
}
//------default_proto--------

static Proto vec[MAX_PROTO] = {
                                {default_proto_match<RpcProtocol>, proto_new<RpcProtocol>, default_proto_parse<RpcProtocol>, default_proto_process<RpcProtocol>, default_proto_response<RpcProtocol>},
                                {default_proto_match<HttpProtocol>, proto_new<HttpProtocol>, default_proto_parse<HttpProtocol>, default_proto_process<HttpProtocol>, default_proto_response<HttpProtocol>}, 
                              };

Proto* select_proto(IoBuf* io_buf)
{
    for (size_t i = 0; i < MAX_PROTO; ++i)
    {
        if (vec[i].match(io_buf) == PARSE_SUCCESS)
        {
            return &vec[i];
        }
    }
    return NULL;
}

}