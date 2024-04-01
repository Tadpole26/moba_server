#include "pch.h"
#include "msg_parser.h"
#include "io_thread.h"
#include "thread_dispatcher.h"
#include "../platform/log_mgr.h"
#include "../platform/util_malloc.h"
#include <event2/bufferevent.h>
#include <event2/buffer.h>

tagMsgHead* make_head_msg(const char* pBuf, size_t size
    , char* arrBuffer, uint8_t usModule, uint32_t uiCmd
    , uint32_t uiSequence, uint32_t eCode)
{
    size_t len = NET_HEAD_SIZE + size;

    tagMsgHead* pNetMsgHead = (tagMsgHead*)(arrBuffer);
    if (pNetMsgHead == nullptr) return nullptr;

    pNetMsgHead->usModuleId = usModule;
    pNetMsgHead->uiCmdId = uiCmd;
    pNetMsgHead->uiCode = eCode;
    pNetMsgHead->uiSeqid = uiSequence;
    pNetMsgHead->uiFlag = GAME_BASE_FLAG;
    pNetMsgHead->uiLen = (uint32_t)len;
    pNetMsgHead->uiCrc = 0;

    char* pBegin = arrBuffer + NET_HEAD_SIZE;

    //只保存头的，直接返回
    if (pBuf == nullptr || size == 0)
        return pNetMsgHead;

    if (len > MSG_MAX_LEN)
    {
        Log_Error("msg size is too long! module:%u, len:%u", usModule, uiCmd, len);
        return nullptr;
    }

    //相同的地址  直接返回
    if (pBegin == pBuf) return pNetMsgHead;
    memcpy_safe(pBegin, size, pBuf, size);
    return pNetMsgHead;
}


static tagNetMsg* my_msg_new(thread_oid_t toid, const conn_oid_t* pCoids
    , uint16_t nCount, const tagMsgHead* pNetMsg)
{
    if (nCount == 0 || pNetMsg == nullptr)
        return nullptr;

    if (pNetMsg->uiLen > MSG_MAX_LEN)
    {
        Log_Error("msg size is too long! module:%u, protol:%u, len:%u"
            , pNetMsg->usModuleId, pNetMsg->uiCmdId, pNetMsg->uiLen);
        return nullptr;
    }

    tagNetMsg* pHostMsg = nullptr;
    tagMsgHead* pMyMsg = nullptr;

    uint32_t nSize = (uint32_t)NET_HOST_SIZE + (uint32_t)(pNetMsg->uiLen);
    if (nCount == 1)
    {
        pHostMsg = net_msg_alloc(nSize);
        if (pHostMsg == nullptr) return nullptr;

        pHostMsg->m_hd.m_type = HMT_NET;
        pHostMsg->m_hd.m_threadOid = toid;
        pHostMsg->m_hd.m_connOid = *pCoids;
        pMyMsg = (tagMsgHead*)pHostMsg->m_body;
    }
    else
    {
        pHostMsg = net_msg_alloc(nSize + sizeof(conn_oid_t) * nCount);
        if (pHostMsg == nullptr) return nullptr;

        pHostMsg->m_hd.m_type = HMT_MUL_NET;
        pHostMsg->m_hd.m_threadOid = toid;
        pHostMsg->m_hd.m_connOid = nCount;

        conn_oid_t* pDstCoids = (conn_oid_t*)pHostMsg->m_body;
        memcpy(pDstCoids, pCoids, sizeof(conn_oid_t) * nCount);

        pMyMsg = (tagMsgHead*)(pDstCoids + nCount);
    }

    if (pNetMsg->uiFlag != GAME_ZIP_FLAG)
        memcpy(pMyMsg, pNetMsg, pNetMsg->uiLen);
    else
    {
        memcpy(pMyMsg, pNetMsg, NET_HEAD_SIZE);
        uchar* pMyBuf = NET_DATA_BUF(pMyMsg);
        uchar* pNetBuf = NET_DATA_BUF(pNetMsg);
        memcpy(pMyBuf, pNetBuf, NET_DATA_SIZE(pNetMsg));
    }
    return pHostMsg;
}

bool my_send_conn_msg(CThreadDispatcher* pDispatcher, thread_oid_t toid
    , conn_oid_t coid, const tagMsgHead* pMsg)
{
    return my_multicast_conn_msg(pDispatcher, toid, &coid, 1, pMsg);
}

bool my_multicast_conn_msg(CThreadDispatcher* pDispatcher, thread_oid_t toid
    , const conn_oid_t* pCoids, uint16_t count, const tagMsgHead* pMsg)
{
    if (toid == invalid_thread_oid || count == 0)
        return false;

    tagNetMsg* pNetMsg = my_msg_new(toid, pCoids, count, pMsg);
    if (pNetMsg == nullptr)
        return false;

    if (!pDispatcher->send_msg_hton(pNetMsg))
    {
        msg_free(pNetMsg);
        return false;
    }
    return true;
}

EParse_Msg_Relt my_io_recv_msg(CNetConn* pConn, CMsgQueue* pInputQueue)
{
    bufferevent* pBuffer = pConn->get_ev_buffer();
    if (pBuffer == nullptr) return pmr_buffer_error;

    for (;;)
    {
        evbuffer* pInputBuf = bufferevent_get_input(pBuffer);
        if (evbuffer_get_length(pInputBuf) < NET_HEAD_SIZE)
            return pmr_ok;

        tagMsgHead msgHd;
        if (evbuffer_copyout(pInputBuf, &msgHd, NET_HEAD_SIZE) != NET_HEAD_SIZE)
            return pmr_buffer_error;

        if ((msgHd.uiFlag >> 12) != 0x8)
            return pmr_buffer_error;

        if (msgHd.uiLen > MSG_MAX_LEN || msgHd.uiLen < NET_HEAD_SIZE)
            return pmr_invalid_size;

        size_t msgSize = msgHd.uiLen;
        if (evbuffer_get_length(pInputBuf) < msgSize)
            return pmr_ok;

        if (pConn->GetParseCode() != pmr_ok)
            return pConn->GetParseCode();

        if (!pConn->recv_sec())
            return pmr_more_failed;

        uint32_t nAlloc = (uint32_t)NET_HOST_SIZE + (uint32_t)msgSize;
        tagNetMsg* pHostMsg = net_msg_alloc(nAlloc);
        if (pHostMsg == nullptr) return pmr_push_failed;

        pHostMsg->m_hd.m_type = HMT_NET;
        pHostMsg->m_hd.m_threadOid = pConn->get_thread_id();
        pHostMsg->m_hd.m_connOid = pConn->get_conn_id();

        size_t readRm = bufferevent_read(pBuffer, &pHostMsg->m_body, msgSize);
        if (readRm != msgSize)
        {
            msg_free(pHostMsg);
            return pmr_buffer_error;
        }

        if (!pInputQueue->push(pHostMsg))
        {
            Log_Error("error!");
            msg_free(pHostMsg);
            return pmr_push_failed;
        }

    }
    return pmr_ok;
}

void my_io_send_msg(const void* pMsg, void* args)
{
    tagNetMsg* pNetMsg = (tagNetMsg*)pMsg;
    const tagHostHd& hd = pNetMsg->m_hd;

    CIoThread* pIoThread = (CIoThread*)args;
    if (hd.m_threadOid != pIoThread->EvThread()->Id())
    {
        Log_Error("send_msg_hton failed(thread not match %u)", (uint32_t)hd.m_threadOid);
        return;
    }
    CNetConnPool* pConnPool = pIoThread->ConnPool();
    if (pConnPool == nullptr)
    {
        Log_Error("io_thread_conn_pool return nullptr");
        return;
    }

    if (hd.m_type == HMT_NET)
    {
        tagMsgHead* pHeadMsg = (tagMsgHead*)(pNetMsg->m_body);
        if (pConnPool->write(hd.m_connOid, pHeadMsg, pHeadMsg->uiLen) != 0)
        {
            Log_Warning("huge(conn_oid=%u), size:%u", hd.m_connOid, pHeadMsg->uiLen);
        }
        else if (pNetMsg->m_hd.m_type == HMT_MUL_NET)
        {
            uint16_t nConnCount = hd.m_connOid;
            conn_oid_t* pCoids = (conn_oid_t*)pNetMsg->m_body;
            tagMsgHead* pHeadMsg = (tagMsgHead*)(pCoids + nConnCount);

            for (uint16_t i = 0; i < nConnCount; ++i)
            {
                conn_oid_t coid = pCoids[i];
                if (pConnPool->write(coid, pHeadMsg, pHeadMsg->uiLen) != 0)
                {
                    Log_Warning("huge(conn_oid=%u), size:%u", coid, pHeadMsg->uiLen);
                }
            }
        }
    }
}