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

     //Э�����
    ParseResult read_event(IoBuf* io_buf, RpcContext* context);

    ParseResult write_event(RpcContext* context);

private:
    //Э��ƥ��
    virtual ParseResult proto_match(IoBuf* io_buf);

    //����Э������Ҫ��context
    virtual void *alloc_proto_ctx(RpcContext* context);

    //�Ƴ�������context
    virtual void del_proto_ctx(void *context);

private:


    void fill_reply_data(::google::protobuf::Message* req_msg, ::google::protobuf::Message* resp_msg);
};

}

#endif
