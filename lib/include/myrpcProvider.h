#pragma once
#include "google/protobuf/service.h"
#include <memory>
#include<muduo/net/TcpServer.h>
#include<muduo/net/EventLoop.h>
#include<muduo/net/InetAddress.h>
#include<muduo/net/TcpConnection.h>
#include <google/protobuf/descriptor.h>
#include<unordered_map>



class MyrpcProvider{


public:
    void NotifyProvider(google::protobuf::Service *service);


    //启动rpc服务节点，开始提供rpc远程网络调用服务
    void Run();

private:
    std::unique_ptr<muduo::net::TcpServer> m_tcpserver;
    muduo::net::EventLoop m_eventLoop;
    void BuildConnect(const muduo::net::TcpConnectionPtr& conn);
    void BuildMessage(const muduo::net::TcpConnectionPtr&, muduo::net::Buffer*, muduo::Timestamp);
    void SendRpcResponse(const muduo::net::TcpConnectionPtr&, google::protobuf::Message*);

    struct ServiceInfo
    {
        google::protobuf::Service *m_service; //服务对象
        std::unordered_map<std::string, const google::protobuf::MethodDescriptor*> m_method_map;

    };
    
    std::unordered_map<std::string, ServiceInfo> m_service_map;
};
