#include "pch.h"
#include "thread_dispatcher.h"
#include <vector>
#include <event2/util.h>
#include <event2/listener.h>
#include <event2/event.h>
#include "msg_queue.h"
#include "net_connection.h"
#include "msg_interface.h"
#include "logic_interface.h"
#include "time_event.h"
#include <mutex>
#include "event_thread.h"
#include "io_thread.h"
#include "id_alloctor.h"
#include "util_malloc.h"
#include "log_mgr.h"

unsigned SP_THREAD_CALL io_thread_fn(void* args)
{
    CIoThread* pThread = (CIoThread*)args;

    CThreadDispatcher* pDispatcher = pThread->GetParent();
    pDispatcher->add_init_count();
    pThread->EvThread()->Loop();
    return 0;
}

CThreadDispatcher::CThreadDispatcher() { }

CThreadDispatcher::~CThreadDispatcher()
{
    fini();
}

int CThreadDispatcher::init(CEventThread* pLogicThread,
    io_thread_setting& conf)
{
    if (!conf.IsValid())
        return -1;
    //初始化io线程数量
    m_pIoThreads = new CIoThread[conf.m_nIos];
    if (m_pIoThreads == nullptr)
        return -1;
    //初始化io线程
    for (thread_oid_t i = 0; i < conf.m_nIos; ++i)
    {
        if (m_pIoThreads[i].Init(i, conf, this, pLogicThread) != 0)
        {
            Log_Error("init pthread io thread error, tid:%d, ios:%u", i, conf.m_nIos);
            return -1;
        }
    }

    m_iThreadNum = conf.m_nIos;
    m_iThreadPos = 0;
    m_fnDispatch = &CThreadDispatcher::dispatch_default;

    m_nMaxConnOfThread = conf.m_ConnsOfIo;
    m_pLogicIF = nullptr;
    return 0;
}

int CThreadDispatcher::stop()
{
    if (m_pIoThreads != NULL) {
        for (thread_oid_t i = 0; i < m_iThreadNum; ++i)
            m_pIoThreads[i].Stop();
    }
    if (m_pLogicIF != nullptr)
        m_pLogicIF->stop();
    return 0;
}

int CThreadDispatcher::fini()
{
    if (m_pIoThreads != NULL) {
        for (thread_oid_t i = 0; i < m_iThreadNum; ++i)
            m_pIoThreads[i].Fini();
        delete[] m_pIoThreads;
        m_pIoThreads = NULL;
    }
    m_iThreadNum = 0;
    m_iThreadPos = 0;
    m_pLogicIF = NULL;
    m_iCurInitCount = 0;
    return 0;
}

thread_oid_t CThreadDispatcher::dispatch_default(CThreadDispatcher* pDispatcher)
{
    if (pDispatcher->m_iThreadPos >= pDispatcher->m_iThreadNum)
        pDispatcher->m_iThreadPos = 0;
    return pDispatcher->m_iThreadPos++;
}

thread_oid_t CThreadDispatcher::dispatch_min(CThreadDispatcher* pDispatcher)
{
    thread_oid_t pos = 0;
    size_t connsize = 0;
    size_t tempsize = 0;
    for (thread_oid_t i = 0; i < pDispatcher->m_iThreadNum; ++i)
    {
        tempsize = pDispatcher->get_io_thread(i)->ConnPool()->conn_size();
        if (tempsize == 0)
            return i;
        if (tempsize == 0 || tempsize < connsize)
        {
            pos = i;
            connsize = tempsize;
        }
    }
    return pos;
}

void CThreadDispatcher::add_init_count()
{
    m_lockInit.lock();
    ++m_iCurInitCount;
    m_condInit.notify_all();
    m_lockInit.unlock();
}

void CThreadDispatcher::install_dispatch(fn_dispatch_thread fn)
{
    m_fnDispatch = fn;
}

bool CThreadDispatcher::is_valid_io_thread(thread_oid_t iThreadOid)
{
    return (iThreadOid >= 0 && iThreadOid < m_iThreadNum);
}

int CThreadDispatcher::start()
{
    for (thread_oid_t i = 0; i < m_iThreadNum; ++i)
    {
        CIoThread* pThread = &m_pIoThreads[i];
        if (pThread->EvThread()->Linked())
            pThread->GetParent()->add_init_count();
        else
            pThread->EvThread()->Start(&io_thread_fn, (void*)pThread);
    }

    std::unique_lock<std::mutex> lck(m_lockInit);
    while (m_iCurInitCount < m_iThreadNum)
    {
        m_condInit.wait(lck);
    }
    return 0;
}

int CThreadDispatcher::dispatch_accpet(evutil_socket_t fd, session_oid_t soid
    , sockaddr addr, uint32_t maxBuffer, EConn_Type accepted)
{
    thread_oid_t iThreadOid = (*m_fnDispatch)(this);
    if (!is_valid_io_thread(iThreadOid))
        return -1;

    CIoThread& ioThread = m_pIoThreads[iThreadOid];
    CMsgQueue& MsgQueue = ioThread.AcceptPut();

    tagAcceptConnMsg* pConn = acc_conn_alloc();
    if (pConn == nullptr) return -1;
    pConn->m_fd = fd;
    pConn->m_connType = accepted;
    pConn->m_soid = soid;
    pConn->m_maxBuffer = maxBuffer;
    pConn->m_addr = addr;

    if (!MsgQueue.push((void*)pConn))
    {
        msg_free(pConn);
        return -1;
    }
    return 0;
}

void CThreadDispatcher::reg_Logic(CLogicInterface* pLogicIF)
{
    if (pLogicIF)
    {
        pLogicIF->initialize(this, m_iThreadNum, m_nMaxConnOfThread);
        m_pLogicIF = pLogicIF;
    }
}

CLogicInterface* CThreadDispatcher::get_Logic()
{
    return m_pLogicIF;
}

bool CThreadDispatcher::send_msg_hton(void* pNetMsg)
{
    if (pNetMsg == nullptr) return false;
    tagHostHd* pHost = (tagHostHd*)pNetMsg;
    thread_oid_t toid = pHost->m_threadOid;

    if (!is_valid_io_thread(toid))
        return false;

    CIoThread& ioThread = m_pIoThreads[toid];
    ioThread.AddPut();
    CMsgQueue& OutputQueue = ioThread.OutPut();
    if (!(OutputQueue.push(pNetMsg)))
        return false;
    return true;
}

thread_oid_t CThreadDispatcher::get_threads()
{
    return m_iThreadNum;
}

CIoThread* CThreadDispatcher::get_io_thread(thread_oid_t toid)
{
    if (is_valid_io_thread(toid))
        return &m_pIoThreads[toid];
    return nullptr;
}

conn_oid_t CThreadDispatcher::get_thread_max_conn()
{
    return m_nMaxConnOfThread;
}