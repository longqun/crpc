#include "http_service.h"
#include "http_req_parse.h"
#include "http_protocol_context.h"

namespace crpc
{

HttpProtocolContext::HttpProtocolContext(RpcContext * context)
                                                                    :_http_read_body(NULL),
                                                                    _context(context), 
                                                                    _http_phase(HTTP_PARSE_LINE_PHASE),
                                                                    _has_send_header(false), 
                                                                    _req_body_buf(NULL)
{}

#define METHOD_NOT_FOUND "Method Not Found!\n"

//TODO: �ڵ����û�����֮ǰ����Ҫ���ӳ�ʱ�Ķ�ʱ����
ParseResult HttpProtocolContext::parse_http(IoBuf* in)
{
    //����Զ��¼�������Ȥ��ô�����ϸ�����û�����!
    if (!_context->has_read_interest())
    {
        crpc_log("no read interest");
        return NEED_NORE_DATA;
    }

    HttpRequestParser http_parser;
    ParseResult result = PARSE_FAILED;

    while (_http_phase != HTTP_PARSE_DONE_PHASE)
    {
        switch (_http_phase){
            case HTTP_PARSE_LINE_PHASE:
                result = http_parser.parse_req_line(_req_line, in);
                if (result == PARSE_SUCCESS && !GET_HTTP_HANDLE(_req_line.method, _req_line.url, _http_user_handle))
                {
                    send_response(404, METHOD_NOT_FOUND, sizeof(METHOD_NOT_FOUND) - 1);
                    return PARSE_SUCCESS;
                }
                break;
            case HTTP_PARSE_HEAD_PHASE:
                result = http_parser.parse_header(_req_header, in);
                break;
            case HTTP_PARSE_BODY_PHASE:
                if (_http_read_body)
                    result = _http_read_body(this, in, _req_header.get_content_length());
                else
                    result = http_parser.parse_body(this, in, _req_header.get_content_length());
            break;
            default:
                break;
        }

        //increase status
        if (result == PARSE_SUCCESS)
            _http_phase += 1;
        else
            return result;
    }
    return write_event(_context);
}

void HttpProtocolContext::write_http_header(int code, RpcContext *context)
{
    if (!_has_send_header)
    {
        http_res_line line(code);
        std::string str = line.to_string();
        _context->context_write(str.c_str(), str.size());

        //TODO ֧��pipeline
        _res_header.append_header(HTTP_CONECTION, HTTP_CONECTION_CLOSE);
        str = _res_header.to_string();
        context->context_write(str.c_str(), str.size());
        _has_send_header = true;
    }
}

//�����������������
void HttpProtocolContext::send_response(int code, const void *data, int len)
{
    if (!_has_send_header)
    {
        //��ǰ�汾ʵ�ֱ���Ҫ�д��ֶ�
        if (!_res_header.has_header(CONTENT_LENGTH))
        {
            _context->set_context_status(CONTEXT_ERROR);
            return;
        }
        write_http_header(code, _context);
    }

    _context->context_write((const char *)data, len);
    _context->set_context_status(CONTEXT_CLOSE);
    reset_context();
}

void HttpProtocolContext::send_data(int code, const void *data, int len)
{
    if (!_has_send_header)
    {
        //��ǰ�汾ʵ�ֱ���Ҫ�д��ֶ�
        if (!_res_header.has_header(CONTENT_LENGTH))
        {
            _context->set_context_status(CONTEXT_ERROR);
            return;
        }
        write_http_header(code, _context);
    }

    _context->context_write((const char *)data, len);
}

ParseResult HttpProtocolContext::write_event(RpcContext *c)
{
    ParseResult result = PARSE_SUCCESS;
    if (_http_phase == HTTP_PARSE_DONE_PHASE)
    {
        //��ֹ���¼����ȴ������
        c->disable_read();
        HTTP_HANDLE_STATUS status = _http_user_handle(this);
        if (status == HTTP_HANDLE_OK)
        {
            c->enable_read();
            c->set_context_status(CONTEXT_CLOSE);
            reset_context();
        }
        else if (status == HTTP_HANDLE_FAILED)
        {
            result = PARSE_FAILED;
            c->set_context_status(CONTEXT_CLOSE);
        }
        else
        {
            result = NEED_NORE_DATA;
            crpc_log("pause request!");
        }
    }
    return result;
}

void HttpProtocolContext::reset_context()
{
    _http_phase = HTTP_PARSE_LINE_PHASE;
    _req_line = http_req_line();
    _req_header.reset();
    _res_header.reset();

    delete _req_body_buf;
    _req_body_buf = NULL;

    if (_user_context.destroy)
    {
        _user_context.destroy(_user_context.context);
        _user_context.context = NULL;
        _user_context.destroy = NULL;
    }
}

}

