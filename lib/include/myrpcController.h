#pragma once
#include <google/protobuf/service.h>
#include <string>

class MyrpcController : public google::protobuf::RpcController
{
public:
    MyrpcController();
    void Reset();
    bool Failed() const;
    std::string ErrorText() const;
    void SetFailed(const std::string& reason);


    void StartCancel();
    bool IsCanceled() const;
    void NotifyOnCancel(google::protobuf::Closure* callback);
private:
    bool m_failed; 
    std::string m_errText;
};