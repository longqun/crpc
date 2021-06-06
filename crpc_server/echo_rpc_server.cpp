#include "crpc/crpc_server.h"
#include "echo.pb.h"
#include "http.pb.h"

using namespace crpc;

#define OUT_WORLD "hello world\n"

//http service
class HttpEchoService : public http::HttpService {
public:
  virtual void Echo(::google::protobuf::RpcController* con,
                       const ::http::HttpRequest* request,
                       ::http::HttpResponse* response,
                       ::google::protobuf::Closure* done) {

    ProtoRpcController* controller = (ProtoRpcController*)con;
    IoBuf& out_buf = controller->get_write_io_buf();
    //you can get form data from body_data
    unsigned char* body_data = controller->get_http_parser()->get_body();

    out_buf.append(OUT_WORLD, strlen(OUT_WORLD));
    done->Run();
  }
};

//rpc service
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

int main()
{
    ServerOption option(8080, "0.0.0.0");
    CRpcServer server;

    MyEchoService echo_service;
    HttpEchoService http_echo_service;

    server.add_service(&echo_service);
    server.add_http_service(&http_echo_service,"/v1/echo/", "Echo");
    server.start(option);
}

