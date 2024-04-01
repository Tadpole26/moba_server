#pragma once

#include "net_define.h"
#include <mutex>
#include "cir_queue.h"
//多进一出

class TCirQueueMul : public TCirQueue
{
public:
    TCirQueueMul() {}
    ~TCirQueueMul() { Release(); }

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
    ///pop队列最前一个消息下标
    int32_t _pop = 0;
    //pop队列当前消息数
    int32_t _pop_count = 0;
    //pop队列
    TElemP* _elemspop = nullptr;
};
