#include "crpc_server.h"
#include "http/http_protocol_context.h"
#include "http/http_service.h"
#include "echo.pb.h"
#include "http.pb.h"
#include "rpc/rpc_service.h"
#include <unistd.h>

using namespace crpc;

/*
    RPC demo
*/
class MyEchoService : public echo::EchoService {
public:
  virtual void Echo(::google::protobuf::RpcController* /* controller */,
                       const ::echo::EchoRequest* request,
                       ::echo::EchoResponse* response,
                       ::google::protobuf::Closure* done) {
      std::cout << request->msg() << std::endl;
      response->set_msg(
              std::string("I have received '") + request->msg() + std::string("'"));
      done->Run();
  }
};

/*
    HTTP demo
*/
#define HELLO_WORLD "hello world"
const std::string s_hello_world(HELLO_WORLD);
struct handle_user_context
{
    int index;
};

void handle_user_destroy(void *ptr)
{
    handle_user_context *context = (handle_user_context *)ptr;
    delete context;
    printf("call handle_user_destroy\n");
}

HTTP_HANDLE_STATUS handle(HttpProtocolContext *http_context)
{
    HttpHeader &header = http_context->get_response_header();

    handle_user_context *handle_user_context_ptr = (handle_user_context *)http_context->get_user_context();
    if (handle_user_context_ptr == NULL)
    {
        HttpUserContext user_context;
        handle_user_context_ptr = new handle_user_context;
        user_context.context = (void *)handle_user_context_ptr;
        user_context.destroy = handle_user_destroy;

        http_context->set_user_context(user_context);
        handle_user_context_ptr->index = 0;
        header.append_header(CONTENT_LENGTH, std::to_string(s_hello_world.size()));
    }

    http_context->send_data(200, &s_hello_world[handle_user_context_ptr->index++], 1);
    if (handle_user_context_ptr->index > s_hello_world.size())
        return HTTP_HANDLE_OK;

    return HTTP_HANDLE_PAUSE;
}

int main()
{
    ServerOption option(8080, "0.0.0.0");
    CRpcServer server;
    MyEchoService echo_service;

    REG_HTTP_HANDLE(HTTP_GET, "/", std::bind(handle, std::placeholders::_1));
    REG_RPC_SERVICE(&echo_service);
    server.start(option);
    return 0;
}

