#pragma once

#include "net_define.h"
#include <mutex>
#include "cir_queue.h"
//���һ��

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
    ///pop������ǰһ����Ϣ�±�
    int32_t _pop = 0;
    //pop���е�ǰ��Ϣ��
    int32_t _pop_count = 0;
    //pop����
    TElemP* _elemspop = nullptr;
};
