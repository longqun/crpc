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
    HTTP_PARSE_LINE_PHASE,      //请求行
    HTTP_PARSE_HEAD_PHASE,      //请求头
    HTTP_PARSE_BODY_PHASE,      //请求body
    HTTP_PARSE_DONE_PHASE       //调用用户函数
};

enum HTTP_HANDLE_STATUS
{
    HTTP_HANDLE_OK,         //可以走之后的流程
    HTTP_HANDLE_FAILED,     //当前流程直接结束了
    HTTP_HANDLE_PAUSE       //阻塞在当前流程，继续回调此函数
};

typedef void (*user_destroy)(void *);
struct HttpUserContext
{
    //销毁函数
    user_destroy destroy;
    void *context;

    HttpUserContext():destroy(NULL), context(NULL)
    {}
};

//用户读取body函数
typedef ParseResult (*http_read_body)(void *http_context, IoBuf *io_buf, uint64_t content_length);
typedef std::function<HTTP_HANDLE_STATUS (HttpProtocolContext *)> http_handle;

//TODO 对外暴露了太多的接口了需要精简一下
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

    //请求body
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

    //解析http协议
    ParseResult parse_http(IoBuf *in);

    //数据发送完成可能会触发重新调用用户代码
    ParseResult write_event(RpcContext *c);

    //封装发送接口
    void send_response(int code, const void *data, int len);

    //封装发送接口，区别是这里不会关闭连接
    void send_data(int code, const void *data, int len);

private:

    //重置上下文
    void reset_context();

    void write_http_header(int code, RpcContext *c);

    http_read_body _http_read_body;

    http_handle _http_user_handle;

    RpcContext* _context;

    //http处理哪个阶段
    int _http_phase;

    //是否发送了http头
    bool _has_send_header;

    //用户定义的context
    HttpUserContext _user_context;

    //请求行
    http_req_line _req_line;

    /*
        请求头
    */
    HttpHeader _req_header;

    /*
        请求body
    */
    char *_req_body_buf;

    /*
        回复头
    */
    HttpHeader _res_header;

};


}
#endif