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

//httpЭ�����
class HttpProtocol : public Protocol
{
public:
    HttpProtocol()
    {}

    ~HttpProtocol()
    {}

    //Э��ƥ��
    virtual ParseResult proto_match(IoBuf* io_buf);

    //����Э������Ҫ��context
    virtual void *alloc_proto_ctx();

    //�Ƴ�������context
    virtual void del_proto_ctx(void *context);

    //Э�����
    virtual ParseResult proto_parse(IoBuf* io_buf, RpcContext* context);

    //�ص�����
    virtual void proto_process(RpcContext* context);

    //Э��ظ�����
    virtual void proto_response(ProtoRpcController* con, RpcContext* context, IoBuf* io_buf);

    //Э��ظ�������ɻص���������������ctx״̬��
    virtual void proto_finished(RpcContext* context);

private:
    void fill_reply_data(::google::protobuf::Message* req_msg, ::google::protobuf::Message* resp_msg);
};

}

#endif
