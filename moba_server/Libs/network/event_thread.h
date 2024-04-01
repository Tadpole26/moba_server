#pragma once
//�¼��߳���(�����߳�,�߳��ڲ������¼�ѭ��)
#include <functional>
#include <vector>
#include <thread>
#include <event2/event_struct.h>
#include "net_define.h"

typedef std::function<unsigned(void*)> fn_Thread;

struct event_base;
class CEventThread
{
public:
    CEventThread();
    ~CEventThread();

    int Init(thread_oid_t iOid);
    int Init(thread_oid_t iOid, event_base* pEvBase);
    int Fini();

    bool Linked() const { return m_bLink; }

    thread_oid_t Id();
    event_base* Base();

    int Start(fn_Thread fnThread, void* args);
    int Stop();
    int Loop();

private:
    thread_oid_t        m_id = invalid_thread_oid;
    event_base*         m_pEvBase = nullptr;
    std::thread         m_thread;
    bool                m_bLink = false;                //�Ƿ��ǵ��߳�
    bool                m_bStop = false;
};

