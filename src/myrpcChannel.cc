#include"myrpcChannel.h"
#include "myrpcHeader.pb.h"
#include <string>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "myrpcApplication.h"

void MyrpcChannel::CallMethod(const google::protobuf::MethodDescriptor* method,
                          google::protobuf::RpcController* controller, 
                          const google::protobuf::Message* request,
                          google::protobuf::Message* response, 
                          google::protobuf::Closure* done)
{

    const google::protobuf::ServiceDescriptor* serviceDes = method->service();
    std::string service_name = serviceDes->name(); // service_name
    std::string method_name = method->name(); // method_name

    // 获取参数的序列化字符串长度 args_size
    uint32_t args_size = 0;
    std::string args_str;
    if (request->SerializeToString(&args_str))
    {
        args_size = args_str.size();
    }
    else
    {
        controller->SetFailed("serialize request error!");
        return;
    }
    
    myrpc::RpcHeader myrpc_header;
    myrpc_header.set_service_name(service_name);
    myrpc_header.set_method_name(method_name);
    myrpc_header.set_args_size(args_size);
    
    uint32_t header_size = 0;
    std::string rpc_header_str;
    if (myrpc_header.SerializeToString(&rpc_header_str))
    {
        header_size = rpc_header_str.size();
    }
    else
    {
        controller->SetFailed("serialize rpc header error!");
        return;
    }

    std::string send_rpc_string;
    send_rpc_string.insert(0, std::string((char *)&header_size, 4));
    send_rpc_string += rpc_header_str;
    send_rpc_string += args_str;


    // 打印调试信息
    std::cout << "============================================" << std::endl;
    std::cout << "header_size: " << header_size << std::endl; 
    std::cout << "rpc_header_str: " << rpc_header_str << std::endl; 
    std::cout << "service_name: " << service_name << std::endl; 
    std::cout << "method_name: " << method_name << std::endl; 
    std::cout << "args_str: " << args_str << std::endl; 
    std::cout << "send_rpc_string: " << send_rpc_string << std::endl; 
    std::cout << "============================================" << std::endl;


    //调用端不需要高并发，简单tcp编程发起调用请求

    int client_fd = socket(AF_INET,SOCK_STREAM,0);
    if(client_fd == -1){
        char errtxt[512] = {0};
        sprintf(errtxt, "create socket error! errno:%d", errno);
        controller->SetFailed(errtxt);
        return;
    }
    std::string ip = MyrpcApplication::GetInstance().GetConfig().SearchInfo("rpcserverip");
    uint16_t port = atoi(MyrpcApplication::GetInstance().GetConfig().SearchInfo("rpcserverport").c_str());
    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    // address.sin_addr.s_addr = inet_addr(ip.c_str());
    servaddr.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &servaddr.sin_addr);
    printf("port:%d \n",ntohs(servaddr.sin_port));

    // 连接rpc服务节点
    if (-1 == connect(client_fd, (struct sockaddr*)&servaddr, sizeof(servaddr)))
    {
        close(client_fd);
        char errtxt[512] = {0};
        sprintf(errtxt, "connect error! errno:%d", errno);
        controller->SetFailed(errtxt);
        return;

    }

    // 发送rpc请求
    if (-1 == send(client_fd, send_rpc_string.c_str(), send_rpc_string.size(), 0))
    {
        close(client_fd);
        char errtxt[512] = {0};
        sprintf(errtxt, "send error! errno:%d", errno);
        controller->SetFailed(errtxt);
        return;

    }

    char recv_buf[1024] = {0};
    int recv_size = 0;
    if (-1 == (recv_size = recv(client_fd, recv_buf, 1024, 0)))
    {
        close(client_fd);
        char errtxt[512] = {0};
        sprintf(errtxt, "recv error! errno:%d", errno);
        controller->SetFailed(errtxt);
        return;
    }
    // std::string response_str(recv_buf, 0, recv_size);
    // if (response->ParseFromString(response_str))
    if (!response->ParseFromArray(recv_buf, recv_size))
    {

        close(client_fd);
        char errtxt[2048] = {0};
        sprintf(errtxt, "response_str parse error:%s", recv_buf);
        controller->SetFailed(errtxt);
        return;
    }
    close(client_fd); 
}