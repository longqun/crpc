#include "rpc_channel.h"
#include "crpc_log.h"
namespace crpc
{

void RpcChannel::CallMethod(const ::google::protobuf::MethodDescriptor* method,
            ::google::protobuf::RpcController* /* controller */,
            const ::google::protobuf::Message* request,
            ::google::protobuf::Message* response,
            ::google::protobuf::Closure*) 
{
    std::string serialzied_data = request->SerializeAsString();

    crpc::RpcMeta rpc_meta;
    rpc_meta.set_service_name(method->service()->name());
    rpc_meta.set_method_name(method->name());
    rpc_meta.set_data_size(serialzied_data.size());

    std::string serialzied_str = rpc_meta.SerializeAsString();
    //|rpc_meta大小（定长4字节)|rpc_meta序列化数据（不定长）|request序列化数据（不定长）

    //pack request
    size_t len = serialzied_str.size();
    write(_socket.fd(), &len, sizeof(len));
    write(_socket.fd(), serialzied_str.c_str(), serialzied_str.size());
    write(_socket.fd(), serialzied_data.c_str(), serialzied_data.size());

    //parse response
    int ret = read(_socket.fd(), &len, sizeof(len));
    if (ret <= 0)
    {
        crpc_log("read failed %d %d", ret, errno);
    }
    std::vector<char> vec(len);
    len = read(_socket.fd(), &vec[0], vec.size());
    if (ret <= 0)
    {
        crpc_log("read failed %d %d", ret, errno);
    }

    response->ParseFromString(std::string(&vec[0], vec.size()));
}

}