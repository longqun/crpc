#include "crpc/crpc_server.h"
#include "http.pb.h"
using namespace crpc;

#define OUT_WORLD "hello world\n"

class MyEchoService : public echo::HttpService {
public:
  virtual void Echo(::google::protobuf::RpcController* con,
                       const ::echo::HttpRequest* request,
                       ::echo::HttpResponse* response,
                       ::google::protobuf::Closure* done) {

    ProtoRpcController* controller = (ProtoRpcController*)con;
    IoBuf& out_buf = controller->get_write_io_buf();
    //you can get form data from body_data
    unsigned char* body_data = controller->get_http_parser()->get_body();

    out_buf.append(OUT_WORLD, strlen(OUT_WORLD));
    done->Run();
  }
};//MyEchoService

int main()
{
    ServerOption option(8080, "0.0.0.0");
    CRpcServer server;

    MyEchoService echo_service;
    server.add_http_service(&echo_service, "/v1/echo", "Echo");
    server.start(option, HTTP_PROTO);
}

