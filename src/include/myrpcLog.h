#pragma once

#include "taskQueue.h"

enum LogStatus{

    INFO,
    ERRORINFO,

};
// 定义宏 LOG_INFO("xxx %d %s", 20, "xxxx")
#define LOG_INFO(logmsgformat, ...) \
    do \
    {  \
        MyrpcLog &loginstance = MyrpcLog::GetInstance(); \
        loginstance.SetLogStatus(INFO); \
        loginstance.StartWriteThread();\
        char c[1024] = {0}; \
        snprintf(c, 1024, logmsgformat, ##__VA_ARGS__); \
        loginstance.Log(c); \
    } while(0) \

#define LOG_ERR(logmsgformat, ...) \
    do \
    {  \
        MyrpcLog &loginstance = MyrpcLog::GetInstance(); \
        loginstance.SetLogStatus(ERRORINFO); \
        loginstance.StartWriteThread();\
        char c[1024] = {0}; \
        snprintf(c, 1024, logmsgformat, ##__VA_ARGS__); \
        loginstance.Log(c); \
    } while(0) \

class MyrpcLog{


public:
    void Log(std::string msg);
    void SetLogStatus(LogStatus status);

    void StartWriteThread();
    static MyrpcLog& GetInstance(){
        return instance;
    }

private:
    int log_status;
    TaskQueue<std::string> m_log_que;
    static MyrpcLog instance;

    MyrpcLog(){};
    MyrpcLog(const MyrpcLog&) = delete;
    MyrpcLog(MyrpcLog&&) = delete;

};

