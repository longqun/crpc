#ifndef RPC_PROTOCAL_H
#define RPC_PROTOCAL_H

#include <string>
#include "io_buf.h"
#include "protocol.h"
#include "rpcmeta.pb.h"
#include "rpc_service.h"
#include "proto_rpc_controller.h"
#include "google/protobuf/service.h"
#include "google/protobuf/stubs/common.h"

namespace crpc
{

#define CRPC_STR "CRPC"
#define CRPC_STR_LEN (4)

//|request   ->    "crpc"(�̶��ַ���)| rpc_meta��С������4�ֽ�)|rpc_meta���л����ݣ���������|request���л����ݣ���������
// response  ->    response_size(����4�ֽ�)|response���к����ݣ���������
//status ��ǰ����״̬
enum RpcParseStatus
{
    INVALID_STATUS,
    INIT,
    AFTER_CRPC,
    META_SIZE,
    META_DATA,
};

struct rpc_header
{
    size_t rpc_meta_size;
    RpcMeta rpc_meta;
    //request
    std::string probuf_str;

    //parse status
    RpcParseStatus status;

    rpc_header():rpc_meta_size(0), status(INIT)
    {}

    void reset()
    {
        rpc_meta_size = 0;
        probuf_str = std::string();
        status = INIT;
    }
};

struct rpc_response
{
    size_t response_len;
    std::string proto_str;

    void reset()
    {
        response_len = 0;
        proto_str = std::string();
    }
};

//�������ר����������rpc��
class RpcProtocol : public Protocol
{
private:
    struct RpcProtocolContext
    {
        rpc_header _header;
        rpc_response _response;
    };
public:
    RpcProtocol()
    {}

    ~RpcProtocol()
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
    void invoke_user_cb(RpcContext* context);
    void fill_reply_data(::google::protobuf::Message* req_msg, ::google::protobuf::Message* resp_msg, RpcContext *ctx);

};

}

#endif
