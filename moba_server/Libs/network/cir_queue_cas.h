#pragma once

#include "net_define.h"
#include "cir_queue.h"
#include <atomic>

//һ��һ��������
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
    //��ǰ��Ϣ��������Ϣ����(�̰߳�ȫ)
    std::atomic<int32_t> _cur_count;
};

