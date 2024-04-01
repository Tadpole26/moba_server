#pragma once

#include <event2/event.h>
#include <mutex>
#include <functional>
#include "net_define.h"
#include "cir_queue_mul.h"
#include "cir_queue_cas.h"
#include "cir_queue_gen.h"
#include "socket_pair_wrapper.h"

typedef void* event_msg_t;
#define event_msg_size sizeof(event_msg_t)

typedef std::function<void(const event_msg_t msg)> fn_event_msg_cb;
class CMsgQueue
{
public:
    CMsgQueue(E_Mq_Type type = EMQ_MUL);
    ~CMsgQueue();

    bool init(size_t maxSize, event_base* pEvBase, fn_event_msg_cb cb, E_Mq_Type type);
    void release();
    bool push(const event_msg_t msg);
    bool push(const char* pMsg, size_t size);
    void pop();
    void stop();
    E_Mq_Type type() { return _type; }

    bool Run(event_msg_t msg);
private:
    E_Mq_Type _type = EMQ_MUL;                      //队列类型
    TCirQueue* _queue = nullptr;                    //队列
    CSocketPairWrapper _socketPair;
    fn_event_msg_cb _fnMsgCb = nullptr;
};