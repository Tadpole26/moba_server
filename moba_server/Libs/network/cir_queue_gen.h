#pragma once

#include "net_define.h"
#include <mutex>
#include "cir_queue.h"

//多进多出
class TCirQueueGen : public TCirQueue
{
public:
    TCirQueueGen() {}
    ~TCirQueueGen() { Release(); }

    bool Init(uint32_t max_count);
    bool Release();
    int Push(const TElemP elem);
    void SerPop();
    TElemP Pop();
    int32_t Size();
    bool Empty();
    bool Full();
    bool Clear();

private:
    std::mutex _mutex;
};
