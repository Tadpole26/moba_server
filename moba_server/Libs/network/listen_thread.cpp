#include "pch.h"
#include "listen_thread.h"
#include <event2/listener.h>
#include <event2/event.h>
#include "msg_queue.h"
#include "net_connection.h"
#include "msg_interface.h"
#include "time_event.h"
#include "io_thread.h"
#include "thread_dispatcher.h"
#ifndef _WIN32
#include <linux/tcp.h>
#endif 
#include "net_opt.h"

CListenThread::CListenThread() { }
CListenThread::~CListenThread() { }



//listen线程监听到有人connect回调函数
void CListenThread::listen_callback(struct evconnlistener* pListener, evutil_socket_t fd,
    struct sockaddr* pSockAddr, int socklen, void* user_data)
{
    CListenThread* pLThread = (CListenThread*)user_data;
    CThreadDispatcher* pDispatcher = pLThread->Dispatcher();
    if (pDispatcher == nullptr)
    {
        evutil_closesocket(fd);
        return;
    }
    //生成accpet消息push到io线程的消息队列中
    if (pDispatcher->dispatch_accpet(fd, invalid_session_oid, *pSockAddr, 0) == -1)
        evutil_closesocket(fd);
}
//listen线程 执行函数(线程start后执行该函数)
unsigned SP_THREAD_CALL CListenThread::listen_thread_fn(void* args)
{
    CListenThread* pThread = (CListenThread*)args;
    if (pThread->EvThread() != nullptr)
        pThread->EvThread()->Loop();
    return 0;
}

//对ip:port进行监听
int CListenThread::AddListen(const char* szIp, uint16_t port)
{
    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    if (szIp == nullptr || strlen(szIp) == 0)
        sin.sin_addr.s_addr = inet_addr((const char*)INADDR_ANY);
    else
        sin.sin_addr.s_addr = inet_addr(szIp);
    sin.sin_port = htons(port);

    evconnlistener* pListener = evconnlistener_new_bind(m_pEvThread->Base(), listen_callback,
        this, LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE, -1, (struct sockaddr*)&sin, sizeof(sin));

    if (!pListener)
        return -1;

    evutil_socket_t fd = evconnlistener_get_fd(pListener);
    CNetOpt::SetCloseExec(fd);

    m_pListener = pListener;
    m_port = port;
    return 0;
}

int CListenThread::Init(CEventThread* pLogicThread, net_setting& conf, CThreadDispatcher* pDispatcher)
{
    m_pEvThread = new CEventThread;
    //单线程处理
    if (conf.MqType() == EMQ_NO)
    {
        if (m_pEvThread->Init(invalid_thread_oid, pLogicThread->Base()) != 0)
            return -1;
    }
    else
    {
        if (m_pEvThread->Init(invalid_thread_oid) != 0)
            return -1;
    }

    //logic to listen
    if (!(m_OutputQueue.init(64, m_pEvThread->Base(),
        std::bind(&CListenThread::msg_out_fn, this, std::placeholders::_1)
        , conf.MqType())))
        return -1;

    if (AddListen(conf.m_strListenIp.c_str(), conf.m_nListenPort) != 0)
        return -1;

    m_port = conf.m_nListenPort;
    m_pDispatcher = pDispatcher;
    m_bStop = false;
    return 0;
}

int CListenThread::Stop()
{
    if (m_bStop) return 0;
    m_bStop = true;

    m_OutputQueue.stop();
    if (m_pEvThread != nullptr)
        m_pEvThread->Stop();
    if (m_pListener != nullptr)
    {
        evconnlistener_disable(m_pListener);
        evconnlistener_free(m_pListener);
        m_pListener = nullptr;
    }
    return 0;
}


CMsgQueue& CListenThread::OutPut()
{
    return m_OutputQueue;
}

int CListenThread::Fini()
{
    m_bStop = true;
    if (m_pListener != nullptr)
    {
        evconnlistener_free(m_pListener);
        m_pListener = nullptr;
    }
    m_port = 0;
    m_pDispatcher = nullptr;
    if (m_pEvThread != nullptr)
        delete m_pEvThread;
    m_pEvThread = nullptr;
    return 0;
}

int CListenThread::Start()
{
    //开启线程,libevent dispatch进行事件循环
    if (m_pEvThread != nullptr)
        return m_pEvThread->Start(&listen_thread_fn, (void*)this);
    return -1;
}

void CListenThread::msg_out_fn(const event_msg_t msg)
{
    tagHostHd* pHostMsgHd = (tagHostHd*)msg;
    if (pHostMsgHd->m_type == HMT_CONN)
    {
        tagConnMsg* pConnMsg = (tagConnMsg*)msg;
        if (pConnMsg->m_body.m_state == cs_net_exit)
            Stop();
    }
}













