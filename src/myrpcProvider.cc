#include "myrpcProvider.h"
#include "myrpcApplication.h"
#include "myrpcHeader.pb.h"
#include <functional>



void MyrpcProvider::NotifyProvider(google::protobuf::Service *service){

    ServiceInfo service_info;
    const google::protobuf::ServiceDescriptor *serviceDes = service->GetDescriptor();

    std::string serviceName = serviceDes->name();
    int methodCnt = serviceDes->method_count();
    // std::cout<<"service name:"<<serviceName<<std::endl;
    for (int i = 0; i < methodCnt; i++)
    {
        const google::protobuf::MethodDescriptor*  methodDes= serviceDes->method(i);
        const std::string & methodName = methodDes->name();

        service_info.m_method_map.insert(std::pair(methodName,methodDes));
        // std::cout<<"method name:"<<methodName<<std::endl;
    }

    service_info.m_service = service;
    m_service_map.insert(std::pair(serviceName, service_info));

}


    //启动rpc服务节点，开始提供rpc远程网络调用服务
void MyrpcProvider::Run(){

    std::string ip = MyrpcApplication::GetInstance().GetConfig().SearchInfo("rpcserverip");
    uint16_t port = atoi(MyrpcApplication::GetInstance().GetConfig().SearchInfo("rpcserverport").c_str());
    
    
    muduo::net::InetAddress address(ip, port);
    // std::cout<<"ip:"<<ip<<std::endl;
    // std::cout<<"port:"<<port<<std::endl;
    muduo::net::TcpServer server(&m_eventLoop, address, "RpcProvider");
    //绑定连接回调和消息读写方法;
    server.setConnectionCallback(std::bind(&MyrpcProvider::BuildConnect, this, std::placeholders::_1));
    server.setMessageCallback(std::bind(&MyrpcProvider::BuildMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    //设置muduo库的线程数
    server.setThreadNum(4);


    //启动网络服务 
    server.start();
    m_eventLoop.loop();
    
}

void MyrpcProvider::BuildConnect(const muduo::net::TcpConnectionPtr& conn){
    if (!conn->connected())
    {
        conn->shutdown();
    }
    

}

void MyrpcProvider::BuildMessage(const muduo::net::TcpConnectionPtr& conn, muduo::net::Buffer* buffer, muduo::Timestamp timeStamp){

    std::string recv_buf = buffer->retrieveAllAsString();


    uint32_t header_size = 0;
    recv_buf.copy((char*)&header_size, 4, 0);
    std::string rpc_header_str = recv_buf.substr(4, header_size);
    myrpc::RpcHeader rpcHeader;
    std::string service_name;
    std::string method_name;
    uint32_t args_size;
    if (rpcHeader.ParseFromString(rpc_header_str))
    {

        service_name = rpcHeader.service_name();
        method_name = rpcHeader.method_name();
        args_size = rpcHeader.args_size();
    }
    else
    {
        std::cout << "rpc_header_str:" << rpc_header_str << " parse error!" << std::endl;
        return;
    }


    std::string args_str = recv_buf.substr(4 + header_size, args_size);

    std::cout << "============================================" << std::endl;
    std::cout << "header_size: " << header_size << std::endl; 
    std::cout << "rpc_header_str: " << rpc_header_str << std::endl; 
    std::cout << "service_name: " << service_name << std::endl; 
    std::cout << "method_name: " << method_name << std::endl; 
    std::cout << "args_str: " << args_str << std::endl; 
    std::cout << "============================================" << std::endl;

    auto ser_iter = m_service_map.find(service_name);
    if(ser_iter == m_service_map.end()){
        std::cout<<service_name<<"is exist"<<std::endl;
        return;

    }
    google::protobuf::Service *service = ser_iter->second.m_service;

    auto method_iter = ser_iter->second.m_method_map.find(method_name);

    if(method_iter == ser_iter->second.m_method_map.end()){
        std::cout<<method_name<<"is exist"<<std::endl;
        return;

    }
    const google::protobuf::MethodDescriptor *method = method_iter->second;

    google::protobuf::Message *request = service->GetRequestPrototype(method).New();
    if (!request->ParseFromString(args_str))
    {
        std::cout << "request parse error, content:" << args_str << std::endl;
        return;
    }
    google::protobuf::Message *response = service->GetResponsePrototype(method).New();

    google::protobuf::Closure *done = google::protobuf::NewCallback<MyrpcProvider, 
                                                                    const muduo::net::TcpConnectionPtr&, 
                                                                    google::protobuf::Message*>
                                                                    (this, 
                                                                    &MyrpcProvider::SendRpcResponse, 
                                                                    conn, response);
                                                                    
    service->CallMethod(method,nullptr,request,response,done);


}


void MyrpcProvider::SendRpcResponse(const muduo::net::TcpConnectionPtr& conn, google::protobuf::Message *response)
{
    std::string response_str;
    if (response->SerializeToString(&response_str))
    {
        conn->send(response_str);
    }
    else
    {
        std::cout << "serialize response_str error!" << std::endl; 
    }
    conn->shutdown();
}