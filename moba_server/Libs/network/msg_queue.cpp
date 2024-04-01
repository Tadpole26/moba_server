#include "pch.h"
#include "msg_queue.h"
#include <event2/event_struct.h>
#include <event2/event.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/listener.h>
#include <event2/util.h>
#include <mutex>
#include <thread>
#include <condition_variable>
#include "../platform/log_mgr.h"
#include "../platform/util_malloc.h"
#include "msg_interface.h"

//////////////////////////////////////////////////////////////////////////////////

static void event_msgqueue_pop(void* args);

CMsgQueue::CMsgQueue(E_Mq_Type type)
    : _type(type)
{}

CMsgQueue::~CMsgQueue()
{
    release();
}

bool CMsgQueue::init(size_t maxSize, event_base* pEvBase, fn_event_msg_cb cb, E_Mq_Type type)
{
    _type = type;
    _fnMsgCb = cb;

    if (_queue == nullptr)
    {
        switch (_type)
        {
        case EMQ_CAS:
            _queue = new TCirQueueCas();
            break;
        case EMQ_MUL:
            _queue = new TCirQueueMul();
            break;
        case EMQ_GEN:
            _queue = new TCirQueueGen();
            break;
        case EMQ_NO:
            return true;
            break;
        default:
            return false;
            break;
        }
    }
    //��ʼ��������Ϣ���ֵ
    if (!_queue->Init((int)maxSize))
        return false;
    //event_msgqueue_pop�ж���Ϣ�ͻ�pop,�Ӷ�����pop�ص�����
    if (!(_socketPair.init(pEvBase, event_msgqueue_pop, this)))
        return false;

    return true;
}
//�ͷ��ڴ�
void CMsgQueue::release()
{
    if (_queue != nullptr)
    {
        _queue->Release();
        delete _queue;
        _queue = nullptr;
    }
    _socketPair.release();
    _fnMsgCb = nullptr;
}
//�������push_backһ����Ϣ
bool CMsgQueue::push(const event_msg_t msg)
{
    if (_queue == nullptr)
    {
        if (Run(msg)) return true;
        return false;
    }
    else
    {
        int res = 0;
        res = _queue->Push(msg);
        int nTimes = 0;
        //push_backʧ��,�Ѿ����������ֵ
        while (res == -1)
        {
            ++nTimes;
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            Log_Error("push max msg_queue_size, times:%d", nTimes);
            res = _queue->Push(msg);
        }
        //�Ӷ���Ϊ�յ�һ����ʱ��,����һ��socketpair��write
        if (res == 1)
            _socketPair.write();
        return true;
    }
}

bool CMsgQueue::push(const char* pMsg, size_t size)
{
    tagBufItem* pNewMsg = buf_item_alloc((int)size + BUF_ITEM_SIZE);
    if (pNewMsg == nullptr) return false;

    pNewMsg->_size = size;
    memcpy(pNewMsg->_body, pMsg, size);
    if (!push(pNewMsg))
    {
        Log_Error("error!");
        msg_free(pNewMsg);
        return false;
    }
    return true;
}
//socketpairֻдһ��,read callback����pop�����Ϣ����������Ϣpop��������
void CMsgQueue::pop()
{
    if (_queue == nullptr) return;
    _queue->SerPop();

    TCirQueueMul::TElemP pItem = nullptr;
    pItem = _queue->Pop();
    while (pItem != nullptr)
    {
        Run(pItem);
        pItem = _queue->Pop();
    }
}

void CMsgQueue::stop()
{
    if (_queue == nullptr)
        return;

    _socketPair.stop();
}

bool CMsgQueue::Run(event_msg_t msg)
{
    if (_fnMsgCb != nullptr)
        _fnMsgCb(msg);

    //msg_free(msg);
    free(msg);
    return true;
}

//����read_cb��Ӧ�Ļص�����
static void event_msgqueue_pop(void* args)
{
    CMsgQueue* pMsgQueue = (CMsgQueue*)args;
    pMsgQueue->pop();
}
