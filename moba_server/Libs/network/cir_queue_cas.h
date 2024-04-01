#pragma once

#include "net_define.h"
#include "cir_queue.h"
#include <atomic>

//一进一出，无锁
class TCirQueueCas : public TCirQueue
{
public:
    TCirQueueCas() {}
    ~TCirQueueCas() { Release(); }

    bool Init(uint32_t max_count);
    bool Release();
    int Push(const TElemP elem);
    void SerPop();
    TElemP Pop();
    int32_t Size();
    bool Empty();
    bool Full();
    bool Clear();

protected:
    //当前消息队列中消息数量(线程安全)
    std::atomic<int32_t> _cur_count;
};

