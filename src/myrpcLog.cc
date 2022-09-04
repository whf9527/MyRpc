#include "myrpcLog.h"
#include<time.h>
#include <iostream>

MyrpcLog MyrpcLog::instance;


void MyrpcLog::StartWriteThread(){

    std::thread writeLogThread([&](){

        while (true)
        {
            time_t now = time(nullptr);
            tm* now_tm = localtime(&now);
            char file_name[128];
            sprintf(file_name, "%d-%d-%d-log.txt", now_tm->tm_year+1900, now_tm->tm_mon+1, now_tm->tm_mday);

            FILE *pf = fopen(file_name, "a+");
            if (pf == nullptr)
            {
                std::cout<<file_name<<" Open Error" <<std::endl;
                exit(EXIT_FAILURE);
            }

            std::string msg = m_log_que.Pop();

            char time_buf[128] = {0};
            sprintf(time_buf, "%d:%d:%d =>[%s] ", 
                    now_tm->tm_hour, 
                    now_tm->tm_min, 
                    now_tm->tm_sec,
                    (log_status == INFO ? "info" : "error"));
            msg.insert(0, time_buf);
            msg.append("\n");

            printf("log: %s", msg.c_str());
            fputs(msg.c_str(), pf);
            fclose(pf);

        }
        
       

    });

    writeLogThread.detach();
 


}

void MyrpcLog::Log(std::string msg){

    std::cout<<"msg:"<<msg<<std::endl;
    m_log_que.Push(msg);
}
//
void MyrpcLog::SetLogStatus(LogStatus status){
    log_status = status;

}