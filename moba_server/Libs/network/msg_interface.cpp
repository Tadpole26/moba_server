#include "pch.h"
#include "msg_interface.h"
#include "listen_thread.h"
#include "net_connection.h"
#include "msg_queue.h"
#include "io_thread.h"
#include "event_thread.h"
#include "thread_dispatcher.h"
#include <iostream>
#include "../platform/util_malloc.h"

int io_notify_logic_conn_msg(CNetConn* pConn, EConn_State state, session_oid_t soid)
{
    CIoThread* pThread = pConn->get_thread();
    if (pThread)
    {
        CMsgQueue& InputQueue = pThread->InPut();

        tagConnMsg* pConnMsg = conn_msg_alloc();
        if (pConnMsg == nullptr) return -1;
        pConnMsg->m_hd.m_type = HMT_CONN;
        pConnMsg->m_hd.m_threadOid = pConn->get_thread_id();
        pConnMsg->m_hd.m_connOid = pConn->get_conn_id();
        pConnMsg->m_body.m_state = state;
        pConnMsg->m_body.m_soid = soid;
        pConnMsg->m_body.m_maxBuffer = 0;
        if (InputQueue.push(pConnMsg))
            return 0;
        else
        {
            std::cout << "error!" << std::endl;
            msg_free(pConnMsg);
        }
    }
    return -1;
}

int logic_notify_io_conn_msg(CThreadDispatcher* pDispatcher, const tagHostHd& hd,
    EConn_State state, session_oid_t soid, uint32_t maxBuffer)
{
    CIoThread* pThread = pDispatcher->get_io_thread(hd.m_threadOid);
    if (pThread == nullptr)
        return -1;

    CMsgQueue& OutputQueue = pThread->OutPut();
    tagConnMsg* pConnMsg = conn_msg_alloc();
    if (pConnMsg == nullptr) return -1;

    pConnMsg->m_hd.m_type = HMT_CONN;
    pConnMsg->m_hd.m_threadOid = hd.m_threadOid;
    pConnMsg->m_hd.m_connOid = hd.m_connOid;
    pConnMsg->m_body.m_state = state;
    pConnMsg->m_body.m_soid = soid;
    pConnMsg->m_body.m_maxBuffer = maxBuffer;
    if (OutputQueue.push(pConnMsg))
        return 0;
    else
    {
        std::cout << "error!" << std::endl;
        msg_free(pConnMsg);
    }
    return -1;
}

int logic_notify_io_conn_msg(CThreadDispatcher* pDispatcher, EConn_State state)
{
    thread_oid_t num = pDispatcher->get_threads();
    tagHostHd hd;
    for (thread_oid_t i = 0; i < num; ++i)
    {
        hd.m_type = HMT_CONN;
        hd.m_threadOid = i;
        logic_notify_io_conn_msg(pDispatcher, hd, state, 0, 0);
    }
    return 0;
}

int logic_notify_listen_conn_msg(CListenThread* pListen, EConn_State state)
{
    CMsgQueue& OutputQueue = pListen->OutPut();

    tagConnMsg* pConnMsg = conn_msg_alloc();
    if (pConnMsg == nullptr) return -1;

    pConnMsg->m_hd.m_type = HMT_CONN;
    pConnMsg->m_body.m_state = state;
    if (OutputQueue.push(pConnMsg))
        return 0;
    else
    {
        std::cout << "error!" << std::endl;
        msg_free(pConnMsg);
    }
    return -1;
}

tagConnMsg* conn_msg_alloc()
{
    uint8_t* p = (uint8_t*)msg_alloc(NET_CONN_SIZE);
    if (p == nullptr) return nullptr;

    tagConnMsg* pMsg = new (p)tagConnMsg();
    return pMsg;
}

tagAcceptConnMsg* acc_conn_alloc()
{
    uint8_t* p = (uint8_t*)msg_alloc(NET_ACCEPT_SIZE);
    if (p == nullptr) return nullptr;

    tagAcceptConnMsg* pMsg = new (p)tagAcceptConnMsg();
    return pMsg;
}

tagNetMsg* net_msg_alloc(uint32_t nsize)
{
    uint8_t* p = (uint8_t*)msg_alloc(nsize);
    if (p == nullptr) return nullptr;

    tagNetMsg* pMsg = new (p)tagNetMsg();
    return pMsg;
}

tagBufItem* buf_item_alloc(uint32_t nsize)
{
    uint8_t* p = (uint8_t*)msg_alloc(nsize);
    if (p == nullptr) return nullptr;

    tagBufItem* pMsg = new (p)tagBufItem();
    return pMsg;
}
















