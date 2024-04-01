#include "pch.h"
#include "event_thread.h"
#include <event2/event.h>
#include <event2/event_struct.h>
#include "net_define.h"

CEventThread::CEventThread()
{
}

CEventThread::~CEventThread()
{
    Fini();
}

int CEventThread::Init(thread_oid_t iOid)
{
    m_pEvBase = event_base_new();
    if (m_pEvBase == nullptr)
        return -1;
    m_id = iOid;
    m_bStop = false;
    return 0;
}

int CEventThread::Init(thread_oid_t iOid, event_base* pEvBase)
{
    m_pEvBase = pEvBase;
    m_id = iOid;
    m_bLink = true;
    m_bStop = false;
    return 0;
}

int CEventThread::Fini()
{
    m_bStop = true;
    if (m_pEvBase == nullptr)
        return 0;
    if (m_pEvBase != nullptr)
    {
        if (!m_bLink) event_base_free(m_pEvBase);
        m_pEvBase = nullptr;
    }
    m_id = invalid_thread_oid;
    if (m_thread.joinable())
        m_thread.join();

    return 0;
}

thread_oid_t CEventThread::Id()
{
    return m_id;
}

event_base* CEventThread::Base()
{
    return m_pEvBase;
}

int CEventThread::Start(fn_Thread fnThread, void* args)
{
    if (!m_bLink)
        m_thread = std::thread(fnThread, args);
    return 0;
}

int CEventThread::Stop()
{
    if (m_bStop) return 0;
    m_bStop = true;

    if (m_bLink) return 0;
    if (m_pEvBase != nullptr)
    {
        int ret = event_base_loopbreak(m_pEvBase);
        return ret;
    }
    return -1;
}

int CEventThread::Loop()
{
    if (m_bLink) return 0;

    if (m_pEvBase != nullptr)
        return event_base_dispatch(m_pEvBase);
    return -1;
}