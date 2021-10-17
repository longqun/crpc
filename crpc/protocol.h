#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "io_buf.h"
namespace crpc
{

class RpcContext;
class ProtoRpcController;

enum ParseResult
{
    NEED_NORE_DATA,
    PARSE_FAILED,
    PARSE_SUCCESS
};

class Protocol
{
public:
    Protocol()
    {
    }

     ~Protocol() {};

    //Э��ƥ��
    virtual ParseResult proto_match(IoBuf* io_buf) = 0;

    //����Э������Ҫ��context
    virtual void *alloc_proto_ctx() = 0;

    //�Ƴ�������context
    virtual void del_proto_ctx(void *context) = 0;

    //Э�����
    virtual ParseResult proto_parse(IoBuf* io_buf, RpcContext* context) = 0;

    //�ص�����
    virtual void proto_process(RpcContext* context) = 0;

    //Э��ظ�����
    virtual void proto_response(ProtoRpcController* con, RpcContext* context, IoBuf* io_buf) = 0;

    //Э��ظ�������ɻص���������������ctx״̬��
    virtual void proto_finished(RpcContext* context) = 0;
};

Protocol *select_proto(IoBuf& buf);

}
#endif
