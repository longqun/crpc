#include "protocol.h"
#include "rpccontext.h"
#include "rpc/rpc_protocol.h"
#include "http/http_protocol.h"

namespace crpc
{

static HttpProtocol s_http_proto;
static RpcProtocol s_rpc_proto;

static Protocol *protocol_vec[] = {
    &s_http_proto,
    &s_rpc_proto,
    NULL
};

Protocol *select_proto(IoBuf &buf)
{
    size_t len = sizeof(protocol_vec) / sizeof(void *);
    for (size_t i = 0; i < len; ++i)
    {
        if (protocol_vec[i] && protocol_vec[i]->proto_match(&buf) == PARSE_SUCCESS)
            return protocol_vec[i];
    }
    return NULL;
}

}