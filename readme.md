# CRPC 
是一个了解rpc如何实现的简单项目。

## 如何使用rpc/http服务？

如果只想使用rpc服务直接参考如下代码。
### step1
```
rpc服务
package echo;

option cc_generic_services = true;

message EchoRequest {
    required string msg = 1;
}

message EchoResponse {
    required string msg = 2;
}

service EchoService {
    rpc Echo(EchoRequest) returns (EchoResponse);
}

http服务
package http;

option cc_generic_services = true;

message HttpRequest {};
message HttpResponse {};

service HttpService {
  rpc Echo(HttpRequest) returns (HttpResponse);
};

```
### step2
```
继承对应的服务 & 写业务代码
rpc服务
class MyEchoService : public echo::EchoService {
public:
  virtual void Echo(::google::protobuf::RpcController* /* controller */,
                       const ::echo::EchoRequest* request,
                       ::echo::EchoResponse* response,
                       ::google::protobuf::Closure* done) {
      response->set_msg(
              std::string("I have received '") + request->msg() + std::string("'"));
      done->Run();
  }
};

http服务
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

Note
1:如果你有数据如json要输出，请从controller->get_write_io_buf()获取输出的iobuf然后往里面写数据即可。
2:如果你有请求的数据在body里面，请从controller->get_http_parser()->get_body()获取body中的数据。
3：获取get请求中的参数暂时不支持。
```

### step3
```
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

Note:
1:http需要指定路径对应的服务  如 当你请求http://192.168.1.1/v1/echo/ 则会调用到对应的Echo（在proto中定义的）
```

## 异步rpc使用
### server使用
```
    //proto实现的接口
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

  //业务代码
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

NOTE:
  1：跟同步接口类似，只需要往对应的数据填充即可，最后调用 done->Run() 方法即可
```

## 定时器的使用
```

  基于eventloop & timerfd 实现，使用map<uint64_t, list<TimerObject>>来存储事件，key = 触发时间戳，value = 需要执行的回调函数

  定时器依赖于eventloop线程，定时任务回调也是执行在eventloop线程。

  void run_at(int time, const functor& func);   //只运行一次，隔time毫秒触发

  void run_every(int time, const functor& func);    //无限运行，每隔time毫秒触发


```