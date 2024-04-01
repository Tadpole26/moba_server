#include "pch.h"
#include "logic_thread.h"
#include <signal.h>
#include "reconn_event.h"
#include "logic_interface.h"
#include "io_thread.h"
#include <thread>
#include <chrono>
#include "func_proc.h"
#include "log_mgr.h"

CLogicThread::CLogicThread()
{
}

CLogicThread::~CLogicThread()
{
    if (m_pDispatcher != nullptr)
        delete m_pDispatcher;
    if (m_pReConnDispatcher != nullptr)
        delete m_pReConnDispatcher;
    if (m_pListenThread != nullptr)
        delete m_pListenThread;
    if (m_pEvThread != nullptr)
        delete m_pEvThread;

    if (m_pTimercon != nullptr)
        delete m_pTimercon;

    m_pListenThread = nullptr;
    m_pEvThread = nullptr;
    m_pDispatcher = nullptr;
    m_pReConnDispatcher = nullptr;
    m_pTimercon = nullptr;
}

bool CLogicThread::create_listener(CThreadDispatcher* pDispatcher, CLogicThread* pThread, net_setting& conf)
{
    m_pListenThread = new CListenThread;
    if (m_pListenThread->Init(pThread->evthread(), conf, pDispatcher) != 0)
        return false;

    if (m_pListenThread->Start() != 0)
        return false;

    return true;
}

bool CLogicThread::create_dispatcher(CThreadDispatcher* pDispatcher, CLogicThread* pThread,
    io_thread_setting& conf)
{
    if (pDispatcher->init(pThread->evthread(), conf) != 0)
        return false;

    if (pDispatcher->start() != 0)
        return false;

    return true;
}

static bool has_listen(const net_setting& setting)
{
    return setting.m_nListenPort > 0;
}

CLogicThread* CLogicThread::newm(net_setting& setting)
{
    CLogicThread* pLogicThread = new CLogicThread;
    if (pLogicThread == nullptr)
        return nullptr;

#ifdef WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif

#ifndef WIN32
    struct sigaction sa;
    sa.sa_handler = SIG_IGN;
    sigaction(SIGPIPE, &sa, 0);
#endif 
    if (setting.MqType() == EMQ_NO)
        setting.InitMq(EMQ_NO);

    //main libevent_thread
    if (!pLogicThread->InitLogic())
        goto err;

    if (has_listen(setting))
    {
        if (is_listen_port(setting.m_nListenPort))
        {
            Log_Error("listen port exist %u", setting.m_nListenPort);
            goto err;
        }

        if (!setting.m_ioThread.IsValid())
        {
            Log_Error("io thread num error ios:%u, conns:%u", setting.m_ioThread.m_nIos, setting.m_ioThread.m_ConnsOfIo);
            goto err;
        }

        pLogicThread->m_pDispatcher = new CThreadDispatcher;
        //初始化dippatcher,让所有io_thread 全部start
        if (!pLogicThread->create_dispatcher(pLogicThread->m_pDispatcher, pLogicThread, setting.m_ioThread))
        {
            Log_Error("create dispatcher io thread error");
            goto err;
        }
        //初始化listen_thread,让listen_thread start
        if (!pLogicThread->create_listener(pLogicThread->m_pDispatcher, pLogicThread, setting))
        {
            Log_Error("create listen thread error");
            goto err;
        }
    }

    //reconn io thread dospatcher
    if (setting.m_reThread.IsValid())
    {
        //初始化reconn dippatcher,让所有io_thread 全部start
        pLogicThread->m_pReConnDispatcher = new CThreadDispatcher;
        if (!pLogicThread->create_dispatcher(pLogicThread->m_pReConnDispatcher, pLogicThread,
            setting.m_reThread))
            goto err;

        if (!pLogicThread->m_pTimercon->init(pLogicThread->evthread()->Base(), true
            , setting.m_ReconSec * 1000
            , std::bind(&CLogicThread::trigger, pLogicThread, std::placeholders::_1)))
            goto err;
    }
    pLogicThread->SetSetting(setting);
    return pLogicThread;
err:
    del(pLogicThread);
    return nullptr;
}

void CLogicThread::del(CLogicThread* pThread)
{
    if (pThread == nullptr) return;

    delete pThread;
    pThread = nullptr;
#ifdef WIN32
    WSACleanup();
#endif
}

bool CLogicThread::InitLogic()
{
    m_pTimercon = new CTimeEvent;
    m_pEvThread = new CEventThread;
    //main libevent_thread
    if (m_pEvThread->Init(0) != 0)
        return false;
    return true;
}

int CLogicThread::run()
{
    return m_pEvThread->Loop();
}

int CLogicThread::stop()
{
    if (m_pTimercon != nullptr)
        m_pTimercon->stop();

    if (m_pListenThread != nullptr) logic_notify_listen_conn_msg(m_pListenThread, cs_net_exit);
    if (m_pDispatcher != nullptr) logic_notify_io_conn_msg(m_pDispatcher, cs_net_exit);
    if (m_pReConnDispatcher != nullptr) logic_notify_io_conn_msg(m_pReConnDispatcher, cs_net_exit);

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    if (m_pListenThread != nullptr) m_pListenThread->Stop();
    if (m_pDispatcher != nullptr) m_pDispatcher->stop();
    if (m_pReConnDispatcher != nullptr) m_pReConnDispatcher->stop();
    if (m_pEvThread != nullptr) m_pEvThread->Stop();

    return 0;
}

CEventThread* CLogicThread::evthread()
{
    return m_pEvThread;
}

////////////////////////////////////////////////////////////////////////////////////

int CLogicThread::add_reconn(CReconnSession* pSession,
    const std::string& strIp, uint16_t nPort, int iSecs, uint32_t maxBuffer)
{
    CLogicInterface* pLIF = m_pReConnDispatcher->get_Logic();
    if (pLIF)
    {
        pLIF->add_pre_session(pSession);
        CReconnEvent* pReconn = new CReconnEvent(m_pEvThread
            , m_pReConnDispatcher, iSecs, maxBuffer);
        if (pReconn)
        {
            pSession->set_reconn_event(pReconn);
            pReconn->init(strIp, nPort);
            return 0;
        }
    }

    return -1;
}

int CLogicThread::remove_reconn(CReconnSession* pSession)
{
    CLogicInterface* pLIF = m_pReConnDispatcher->get_Logic();
    if (pLIF)
    {
        return pLIF->kill_pre_session(pSession);
    }
    return -1;
}

bool CLogicThread::send_listen(tagNetMsg* pNetMsg)
{
    return m_pDispatcher->send_msg_hton(pNetMsg);
}

bool CLogicThread::send_reconn(tagNetMsg* pNetMsg)
{
    return m_pReConnDispatcher->send_msg_hton(pNetMsg);
}

void CLogicThread::reg_interface_listen(CLogicInterface* pLogicOps)
{
    if (m_pDispatcher != nullptr)
        m_pDispatcher->reg_Logic(pLogicOps);
}

void CLogicThread::reg_interface_reconn(CLogicInterface* pLogicOps)
{
    if (pLogicOps != nullptr)
        m_pReConnDispatcher->reg_Logic(pLogicOps);
}

void CLogicThread::trigger(uint32_t dwTM)
{
    if (m_pReConnDispatcher != nullptr && m_pReConnDispatcher->get_Logic() != nullptr)
    {
        std::map<session_oid_t, CReconnSession*> mapPre = m_pReConnDispatcher->get_Logic()->get_pre_sessions();
        for (auto item = mapPre.begin(); item != mapPre.end(); ++item)
        {
            item->second->get_reconn_event()->trigger(dwTM);
        }
    }
}

CLogicInterface* CLogicThread::GetInterFace()
{
    if (m_pDispatcher == nullptr) return nullptr;

    return m_pDispatcher->GetInterFace();
}

CLogicInterface* CLogicThread::GetReConnInterFace()
{
    if (m_pReConnDispatcher == nullptr) return nullptr;

    return m_pReConnDispatcher->GetInterFace();
}

CThreadDispatcher* CLogicThread::Dispatcher()
{
    return m_pDispatcher;
}

CThreadDispatcher* CLogicThread::ReDispatcher()
{
    return m_pReConnDispatcher;
}











