#pragma once

#include "net_define.h"
#include <mutex>
//多进多出
class TCirQueue
{
public:
    typedef void* TElemP;

    TCirQueue() {}
    virtual ~TCirQueue() {}

    virtual bool Init(uint32_t max_count) = 0;
    virtual bool Release() = 0;
    virtual int Push(const TElemP elem) = 0;
    virtual void SerPop() = 0;
    virtual TElemP Pop() = 0;
    virtual int32_t Size() = 0;
    virtual bool Empty() = 0;
    virtual bool Full() = 0;
    virtual bool Clear() = 0;

protected:
    //消息队列最前一个消息下标
    int32_t   _head = 0;
    //消息队列最后一个消息+1的下标
    int32_t   _tail = 0;
    int32_t   _cur_count = 0;
    //消息队列可存储消息最大值
    int32_t   _max_count = 0;
    TElemP* _elems = nullptr;
};
