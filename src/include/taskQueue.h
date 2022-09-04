#pragma once

#include<queue>
#include<mutex>
#include<thread>
#include<condition_variable>


template<typename T>
class TaskQueue{

public:
    void Push(const T &data);
    T Pop();


private:
    std::queue<T> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_cond;


};

template<typename T>
void TaskQueue<T>::Push(const T &data){

    std::lock_guard<std::mutex> lock(m_mutex);
    m_queue.push(data);
    m_cond.notify_one();



}

template<typename T>
T TaskQueue<T>::Pop(){

    std::unique_lock<std::mutex> lock(m_mutex);
    while (m_queue.empty())
    {
        m_cond.wait(lock);

    }
    T data = m_queue.front();
    m_queue.pop();
    return data;

}