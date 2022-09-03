#include "myrpcConfig.h"
#include<iostream>
#include<string>

#include <unordered_map>
// #include <utility>
void MyrpcConfig::ConfigLoad(const char *config_file){


    FILE *pf = fopen(config_file, "r");
    if (pf == nullptr)
    {
        std::cout<<config_file<<" is not exist"<<std::endl;
        exit(EXIT_FAILURE);
    }
    

    while (!feof(pf))
    {
            char buf[1024] = {0};
            fgets(buf,1024,pf);

            std::string src_buf(buf);

            DelSpace(src_buf);

            //去除注释 空行
            if (src_buf[0] == '#' && src_buf.empty())
            {
                continue;
            }
            //解析配置
            int idx = src_buf.find("=");
            if (idx == -1)
            {
               continue;
            }
            
            std::string key;
            std::string val;
            key = src_buf.substr(0,idx);
            DelSpace(key);

            int end_idx = src_buf.find('\n',idx);
            val = src_buf.substr(idx+1,end_idx-idx-1);
            DelSpace(val);

            config_map.insert(std::pair(key,val));   
    }

    fclose(pf);


}

std::string MyrpcConfig::SearchInfo(const std::string &key){

    auto iter = config_map.find(key);

    //表示不存在
    if (iter == config_map.end())
    {
            return "";
    }
    
    return iter->second;
    
}

void MyrpcConfig::DelSpace(std::string &buf){

    //去掉前后空格
    int idx = buf.find_first_not_of(" ");
    if (idx != -1)
    {
        buf  = buf.substr(idx, buf.size()-idx);
    }

    idx = buf.find_last_not_of(" ");

    if (idx != -1)
    {
        buf  = buf.substr(0, idx + 1);
    }
    
}