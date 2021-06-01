#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "io_buf.h"
namespace crpc
{

class RpcContext;

//Ŀǰ֧�ֵ�Э��rpc,http
enum ProtoType
{
    RPC_PROTO,
    HTTP_PROTO,
    MAX_PROTO
};

enum ParseResult
{
    NEED_NORE_DATA,
    PARSE_FAILED,
    PARSE_SUCCESS
};

//������ ������ -> ���� -> д��ȥ��
struct Proto
{
    typedef void* (*proto_new_obj)();
    proto_new_obj proto_new;

    typedef ParseResult (*proto_parse)(IoBuf* io_buf, void* user_data);
    proto_parse parse;

    typedef void (*proto_process)(void* user_data);
    proto_process process;

    typedef void (*proto_response)(RpcContext* context, IoBuf* io_buf, void* user_data);
    proto_response response;
};

Proto* get_proto(ProtoType type);

}
#endif
