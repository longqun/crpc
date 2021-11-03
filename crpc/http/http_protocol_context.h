#ifndef CRPC_HTTP_PROTOCOL_CONTEXT_H_
#define CRPC_HTTP_PROTOCOL_CONTEXT_H_

#include <functional>
#include "http_header.h"
#include "rpccontext.h"
#include "http_common_header.h"
#include "io_buf.h"

namespace crpc
{

class HttpProtocolContext;

enum HTTP_PHASE
{
    HTTP_PARSE_LINE_PHASE,      //������
    HTTP_PARSE_HEAD_PHASE,      //����ͷ
    HTTP_PARSE_BODY_PHASE,      //����body
    HTTP_PARSE_DONE_PHASE       //�����û�����
};

enum HTTP_HANDLE_STATUS
{
    HTTP_HANDLE_OK,         //������֮�������
    HTTP_HANDLE_FAILED,     //��ǰ����ֱ�ӽ�����
    HTTP_HANDLE_PAUSE       //�����ڵ�ǰ���̣������ص��˺���
};

typedef void (*user_destroy)(void *);
struct HttpUserContext
{
    //���ٺ���
    user_destroy destroy;
    void *context;

    HttpUserContext():destroy(NULL), context(NULL)
    {}
};

//�û���ȡbody����
typedef ParseResult (*http_read_body)(void *http_context, IoBuf *io_buf, uint64_t content_length);
typedef std::function<HTTP_HANDLE_STATUS (HttpProtocolContext *)> http_handle;

//TODO ���Ⱪ¶��̫��Ľӿ�����Ҫ����һ��
class HttpProtocolContext
{
public:
    HttpProtocolContext(RpcContext* context);

    const HttpHeader &get_req_header()
    {
        return _req_header;
    }

    HttpHeader &get_response_header()
    {
        return _res_header;
    }

    void set_body(char *body)
    {
        _req_body_buf = body;
    }

    //����body
    const char *get_body() const
    {
        return _req_body_buf;
    }

    void set_req_body(char *body)
    {
        _req_body_buf = body;
    }

    void set_user_context(const HttpUserContext &context)
    {
        _user_context = context;
    }

    void *get_user_context()
    {
        return _user_context.context;
    }

    //����httpЭ��
    ParseResult parse_http(IoBuf *in);

    //���ݷ�����ɿ��ܻᴥ�����µ����û�����
    ParseResult write_event(RpcContext *c);

    //��װ���ͽӿ�
    void send_response(int code, const void *data, int len);

    //��װ���ͽӿڣ����������ﲻ��ر�����
    void send_data(int code, const void *data, int len);

private:

    //����������
    void reset_context();

    void write_http_header(int code, RpcContext *c);

    http_read_body _http_read_body;

    http_handle _http_user_handle;

    RpcContext* _context;

    //http�����ĸ��׶�
    int _http_phase;

    //�Ƿ�����httpͷ
    bool _has_send_header;

    //�û������context
    HttpUserContext _user_context;

    //������
    http_req_line _req_line;

    /*
        ����ͷ
    */
    HttpHeader _req_header;

    /*
        ����body
    */
    char *_req_body_buf;

    /*
        �ظ�ͷ
    */
    HttpHeader _res_header;

};


}
#endif