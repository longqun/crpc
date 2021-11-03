#include "rpc/proto_rpc_controller.h"
#include "echo.pb.h"
#include "rpc/rpc_channel.h"

using namespace crpc;
void* call_rpc(void *)
{
    RpcChannel channel;
    channel.init("0.0.0.0", 8080);

    echo::EchoRequest request;
    echo::EchoResponse response;
    request.set_msg("hello, myrpc.");

    echo::EchoService_Stub stub(&channel);
    ProtoRpcController cntl(NULL);
    while (true)
    {
        stub.Echo(&cntl, &request, &response, NULL);
        if (response.msg().empty())
        {

        }
        std::cout << "resp:" << response.msg() << std::endl;
    }
    return NULL;
}

int main()
{
    for (size_t i = 0;i < 10; ++i)
    {
        pthread_t ntid;
        pthread_create(&ntid, NULL, call_rpc, NULL);
    }
    while(true)
    {
        sleep(10000);
    }
    return 0;
}
