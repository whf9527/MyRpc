#pragma once

#include "myrpcConfig.h"

//Myrpc框架初始化类, 单例模式设计只需要一次使用, 饿汉模式。

class MyrpcApplication{


public:
    static void Init(int argc, char** argv);
    static MyrpcApplication& GetInstance() {
		return instance;
	};
    static MyrpcConfig& GetConfig(){
        return m_config;
    };


private:
    static MyrpcConfig m_config;
    static MyrpcApplication instance;

private:

    MyrpcApplication(){}
    MyrpcApplication(const MyrpcApplication&) = delete;
    MyrpcApplication(MyrpcApplication&&) = delete;
    

};
