#include "myrpcApplication.h"
#include <iostream>
#include <unistd.h>
#include <string>

//静态变量初始化

// MyrpcApplication* MyrpcApplication::instance = new MyrpcApplication();
MyrpcApplication MyrpcApplication::instance;
MyrpcConfig MyrpcApplication::m_config;
void ShowArgsHelp(){
    std::cout<<"format: cmd -i <configfile> like ?"<<std::endl;
}

void MyrpcApplication::Init(int argc, char **argv){

    if (argc < 2)
    {
            ShowArgsHelp();
            exit(EXIT_FAILURE);
    }

    int c = 0;

    std::string config_file;
    while((c = getopt(argc, argv, "i:")) != -1)
    {
        switch (c)
        {
        case 'i':
            config_file = optarg;
            break;
        case '?':
            // std::cout<<"arg invalid !"<<std::endl;
            ShowArgsHelp();
            exit(EXIT_FAILURE);
        case ':':
            // std::cout<<"lack <configfile> !"<<std::endl;
            ShowArgsHelp();
            exit(EXIT_FAILURE);
        default:
            break;
        }
    }
    
    m_config.ConfigLoad(config_file.c_str());


    std::cout<<"rpcserverip"<<m_config.SearchInfo("rpcserverip") <<std::endl;
    std::cout<<"rpcserverport"<<m_config.SearchInfo("rpcserverport") <<std::endl;
    std::cout<<"zookeeperip"<<m_config.SearchInfo("zookeeperip") <<std::endl;
    std::cout<<"zookeeperport"<<m_config.SearchInfo("zookeeperport") <<std::endl;


}


