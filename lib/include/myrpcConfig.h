#pragma once 
#include<string>
#include<unordered_map>


class MyrpcConfig{

public:
    void ConfigLoad(const char *config_file);

    std::string SearchInfo(const std::string &key); 

private:
    std::unordered_map<std::string, std::string> config_map;
    
    void DelSpace(std::string &buf);

};