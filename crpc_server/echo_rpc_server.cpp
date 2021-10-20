#include "crpc/crpc_server.h"
#include "echo.pb.h"
#include "http.pb.h"
#include <unistd.h>

using namespace crpc;

#define OUT_WORLD "hello world\n"

struct wrap_obj
{
    ProtoRpcController* con;
    ::google::protobuf::Closure *done;
};

void* thread_out_put(void* arg)
{
    pthread_detach(pthread_self());
    wrap_obj* obj = (wrap_obj*) arg;
    IoBuf& out_buf = obj->con->get_write_io_buf();
    /**/
    out_buf.append(OUT_WORLD, strlen(OUT_WORLD));
    sleep(2);
    obj->done->Run();
    delete obj;
    return NULL;
}

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

    virtual void AsynEcho(::google::protobuf::RpcController* con,
                       const ::http::HttpRequest* request,
                       ::http::HttpResponse* response,
                       ::google::protobuf::Closure* done) {

    wrap_obj *obj = new wrap_obj();
    obj->con = (ProtoRpcController*)con;
    obj->done = done;

    pthread_t ntid;
    int err = pthread_create(&ntid, NULL, thread_out_put, obj);
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

void timer_callback()
{
    printf("tick \n");
}

void timer_callback1()
{
    printf("timer_callback1 \n");
}


int main()
{
    ServerOption option(8080, "0.0.0.0");
    CRpcServer server;

    MyEchoService echo_service;
    HttpEchoService http_echo_service;

    server.add_service(&echo_service);
    server.add_http_service(&http_echo_service,"/v1/echo/", "Echo");
    server.add_http_service(&http_echo_service,"/v1/AsynEcho/", "AsynEcho");
    server.timer_run_every(1000, server.get_main_loop(), std::bind(timer_callback));
    server.timer_run_at(5000, server.get_main_loop(), std::bind(timer_callback1));
    server.start(option);
}

